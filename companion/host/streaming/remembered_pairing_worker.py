"""User-operated remembered service. Startup/help never bind, enroll or arm audio."""
import argparse
import asyncio
from contextlib import asynccontextmanager
import hashlib
from datetime import datetime
import re
import json
import logging
from pathlib import Path
import signal
import ssl
import sys
import time
from remembered_pairing import RememberedRegistry,RememberedPairing,RememberedGateway,DEVICE,URI
from numeric_pairing import strict_json
from connection_recording import ConnectionRecording,record_event

ROOT=Path('/Users/mikedemott/Developer/AgentWork/StackChan')
PROFILE=ROOT/'.tools/persistent-companion/profile.json'

def parse_command(line):
    try:value=strict_json(line)
    except Exception:raise ValueError("invalid command") from None
    if type(value) is not dict or value.get('command') not in ('enroll','confirm','cancel','connect','forget'):raise ValueError('invalid command')
    if value['command']=='connect':
        if set(value)!={'command','audio_route'}:raise ValueError('invalid command')
        from native_conversation import HostAudioDeviceRoute
        HostAudioDeviceRoute.from_dict(value['audio_route'])
    elif set(value)!={'command'}:raise ValueError('invalid command')
    return value['command']

def load_trust_profile(profile_path=PROFILE):
    # Public trust verifies before private-key loading. No live credential generated.
    path=Path(profile_path)
    if path.resolve()!=PROFILE.resolve() or path.is_symlink():raise ValueError('unpinned profile')
    profile=strict_json(path.read_text())
    if set(profile)!={'exact_device_id','host','port','pair_path','audio_path','ca_path','ca_sha256','cert_path','key_path','remember_until_unix','state_directory','transport_scopes','version','begin','end','not_after_utc','approval_sha256','ca_private_key_serialized','profile_activated','automatic_renewal'} or profile['exact_device_id']!=DEVICE or profile['host']!='192.168.1.75' or type(profile['port']) is not int or profile['port']!=18443 or profile['pair_path']!='/companion/pair' or profile['audio_path']!='/companion/audio' or profile['transport_scopes']!=['usb-tls-pcm-v1','wifi-wss']:raise ValueError('invalid profile')
    if type(profile['version']) is not int or profile['version']!=1 or type(profile['begin']) is not int or type(profile['end']) is not int or profile['end']!=profile['remember_until_unix'] or not 0<profile['end']-profile['begin']<=366*86400 or any(profile[k] is not False for k in ('ca_private_key_serialized','profile_activated','automatic_renewal')) or type(profile['approval_sha256']) is not str or not re.fullmatch('[a-f0-9]{64}',profile['approval_sha256']):raise ValueError('invalid authority audit')
    try:audit_expiry=int(datetime.fromisoformat(profile['not_after_utc']).timestamp())
    except Exception:raise ValueError('invalid audit expiry') from None
    if audit_expiry!=profile['end']:raise ValueError('invalid audit expiry')
    base=PROFILE.parent.resolve()
    ca=Path(profile['ca_path']);cert=Path(profile['cert_path']);key=Path(profile['key_path']);state=Path(profile['state_directory'])
    if any(p.is_symlink() or p.resolve().parent!=base for p in (ca,cert,key,state)):raise ValueError('unpinned profile files')
    if ca.stat().st_size>8192 or cert.stat().st_size>8192 or hashlib.sha256(ca.read_bytes()).hexdigest()!=profile['ca_sha256']:raise ValueError('trust changed')
    decoded=ssl._ssl._test_decode_cert(str(cert))
    ca_decoded=ssl._ssl._test_decode_cert(str(ca))
    expiry=int(ssl.cert_time_to_seconds(decoded['notAfter']))
    if type(profile['remember_until_unix']) is not int or profile['remember_until_unix']!=expiry or int(ssl.cert_time_to_seconds(ca_decoded['notAfter']))!=expiry or int(ssl.cert_time_to_seconds(decoded['notBefore']))!=profile['begin'] or not ssl.cert_time_to_seconds(decoded['notBefore'])<=time.time()<expiry:raise ValueError('certificate expired or changed')
    if ('IP Address','192.168.1.75') not in decoded.get('subjectAltName',()):raise ValueError('wrong endpoint certificate')
    if key.stat().st_mode & 0o077 or base.stat().st_mode & 0o077:raise ValueError('private profile required')
    context=ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER);context.minimum_version=ssl.TLSVersion.TLSv1_2;context.load_cert_chain(cert,key)
    return context,profile,state

class RememberedRuntime:
    def __init__(self,profile_path=PROFILE):
        self.context,profile,state=load_trust_profile(profile_path)
        self.registry=RememberedRegistry(state,profile['ca_sha256'],profile['remember_until_unix'])
    @asynccontextmanager
    async def listener(self,gateway):
        from websockets.asyncio.server import serve
        logger=logging.getLogger('stackchan.remembered.private');logger.disabled=True;logger.propagate=False
        async with serve(gateway.handle,'192.168.1.75',18443,ssl=self.context,process_request=gateway.process_request,max_size=4112,max_queue=2,compression=None,ping_interval=None,open_timeout=2,close_timeout=.25,server_header=None,logger=logger):yield

class RememberedWorker:
    def __init__(self,runtime_loader,*,emit,diagnostic_sink=None):
        self.diagnostic_sink=diagnostic_sink
        self.runtime_loader=runtime_loader;self.emit=emit;self.runtime=None;self.pairing=None;self.gateway=None;self.task=None;self.cleanup_blocked=False;self.revocation_pending=False;self._activity=None
    def status(self,state,**extra):self.emit(dict(state=state,**extra))
    def load(self):
        if self.runtime is None:self.runtime=self.runtime_loader()
        return self.runtime
    async def command(self,command,audio_route=None):
        if command not in ('enroll','confirm','cancel','connect','forget'):raise ValueError('invalid command')
        if command!='connect' and audio_route is not None:raise ValueError('invalid command')
        if command in ('cancel','forget'):
            revocation_error=None
            if command=='forget':
                try:self.load().registry.forget()
                except BaseException as failure:self.revocation_pending=True;revocation_error=failure
                else:self.revocation_pending=False
            try:await self.stop()
            finally:
                if self.revocation_pending:self.cleanup_blocked=True;self.status('failed',message='cleanup_unconfirmed',cleanup=['grant'])
            if revocation_error is not None:raise ValueError('revocation unconfirmed') from None
            self.status('cancelled',**({'message':'remembered_forgotten'} if command=='forget' else {}));return
        if self.cleanup_blocked:raise ValueError('cleanup unconfirmed')
        if command=='confirm':
            if self.pairing is None:raise ValueError('comparison required')
            self.pairing.mac_confirm();return
        if self.task and not self.task.done():raise ValueError('busy')
        if command=='connect':
            from native_conversation import HostAudioDeviceRoute,host_audio_device_gateway
            route=HostAudioDeviceRoute.from_dict(audio_route)
            runtime=self.load()
            if runtime.registry.status()!='active':raise ValueError('enrollment required')
            from home_lab.sessions import ConversationSessions
            self.gateway=host_audio_device_gateway(runtime.registry,ConversationSessions(),route);self.gateway.diagnostic_sink=self.diagnostic_sink;self.pairing=None
            self.task=asyncio.create_task(self.listen(None));return
        runtime=self.load();self.pairing=RememberedPairing(runtime.registry,emit=self.emit);self.pairing.enroll()
        self.gateway=RememberedGateway(self.pairing);self.task=asyncio.create_task(self.listen(120))
    async def listen(self,budget):
        async def serve_for_budget():
            async with self.runtime.listener(self.gateway):
                if self.pairing is None:
                    record_event(self.diagnostic_sink,'listener','ready')
                    self._activity=None;self.publish_activity()
                while True:
                    if self.pairing:
                        self.pairing.tick()
                        if self.pairing.state in ('paired','cancelled'):break
                    else:
                        if getattr(self.gateway,'completion_started',False):
                            # Keep the trusted TLS listener ready. The next epoch
                            # stays muted until a fresh physical Listen arrives.
                            self.gateway.completion_started=False
                            self._activity=None
                        self.publish_activity()
                    await asyncio.sleep(.05)
        try:
            if budget is None:await serve_for_budget()
            else:
                async with asyncio.timeout(budget):await serve_for_budget()
        except asyncio.CancelledError:raise
        except Exception:
            record_event(self.diagnostic_sink,'listener','setup_failed')
            self.status('failed',message='setup_or_transport_failed')
        finally:
            if self.pairing is None:record_event(self.diagnostic_sink,'listener','closed')
            gateway_owner=self.gateway
            try:await self.gateway.close()
            except BaseException:
                self.cleanup_blocked=True;self.status('failed',message='cleanup_unconfirmed',cleanup=['audio' if self.pairing is None else 'grant'])
            else:self.gateway=None
            if self.pairing is None and not self.cleanup_blocked:
                result=getattr(gateway_owner,'last_result',{})
                complete=type(result) is dict and result.get('outcome')=='complete' and result.get('consent_received') is True and result.get('conversation_complete') is True
                self.status('closed' if complete else 'failed',message='audio_complete' if complete else 'audio_transport_failed')
    def publish_activity(self):
        activity=getattr(self.gateway,'activity_state','physical_consent')
        if activity not in ('physical_consent','conversation'):activity='physical_consent'
        remaining=getattr(self.gateway,'audio_remaining',None) if activity=='conversation' else None
        if type(remaining) is not int or not 0<=remaining<=60:remaining=None
        snapshot=(activity,remaining)
        if self._activity==snapshot:return
        self._activity=snapshot
        value=dict(acknowledged=True,activity=activity)
        if remaining is not None:value['audio_remaining']=remaining
        self.status('paired',**value)
    async def stop(self):
        if self.task and not self.task.done():self.task.cancel();await asyncio.gather(self.task,return_exceptions=True)
        if self.gateway is not None:
            try:await self.gateway.close()
            except BaseException:
                self.cleanup_blocked=True;self.status('failed',message='cleanup_unconfirmed',cleanup=['audio' if self.pairing is None else 'grant']);raise
            else:self.gateway=None
        self.cleanup_blocked=self.revocation_pending
    async def close(self):
        await self.stop()
        if self.revocation_pending:
            self.load().registry.forget();self.revocation_pending=False;self.cleanup_blocked=False
        if self.runtime is not None:self.runtime.registry.store._connection.close()

def native_route():
    raise RuntimeError('legacy file-based route admission is disabled')

def output(value):
    try:print(json.dumps(value,separators=(',',':')),flush=True)
    except (BrokenPipeError,OSError):pass # Owner cleanup does not depend on UI pipe custody.

async def shutdown_owned(worker,*,sleep=asyncio.sleep):
    """Retain process and exact owned resources until checked closure succeeds."""
    while True:
        try:
            await worker.close()
            if worker.gateway is not None or (worker.task is not None and not worker.task.done()) or worker.revocation_pending:raise RuntimeError('owned closure unconfirmed')
        except BaseException:
            worker.cleanup_blocked=True
            worker.status('failed',message='cleanup_unconfirmed',cleanup=['audio' if worker.gateway is not None and worker.pairing is None else 'grant'])
            # Retry only close/revocation on existing owners; never acquire a route.
            try:await sleep(.25)
            except BaseException:pass
        else:
            worker.status('closed',message='remembered_cleanup_verified');return

async def run():
    worker=RememberedWorker(RememberedRuntime,emit=output,diagnostic_sink=ConnectionRecording(ROOT/'.tools/numeric-ui/connection-diagnostics.json'))
    worker.status('idle')
    reader=asyncio.StreamReader(limit=2048);protocol=asyncio.StreamReaderProtocol(reader)
    loop=asyncio.get_running_loop();pipe,_=await loop.connect_read_pipe(lambda:protocol,sys.stdin)
    for sig in (signal.SIGTERM,signal.SIGINT):loop.add_signal_handler(sig,pipe.close)
    try:
        while True:
            line=await reader.readline()
            if not line:break
            try:
                command=parse_command(line.decode())
                payload=strict_json(line.decode())
                await worker.command(command,payload.get('audio_route'))
            except Exception:worker.status('failed',message='setup_or_transport_failed')
    finally:
        pipe.close()
        await shutdown_owned(worker)
        for sig in (signal.SIGTERM,signal.SIGINT):loop.remove_signal_handler(sig)

def main(argv=None):
    argparse.ArgumentParser(description=__doc__).parse_args(argv)
    try:asyncio.run(run())
    except BaseException:
        output({'state':'failed','message':'setup_or_transport_failed'});return 1
    return 0
if __name__=='__main__':raise SystemExit(main())
