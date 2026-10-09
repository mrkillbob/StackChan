"""JSON-lines numeric-pairing worker; startup/help/import are inert.

Only an explicitly approved, unpaused human prepare command opens USB. No
credential entry, secret stdout, microphone recording or Mac audio devices.
"""
import argparse
import asyncio
from contextlib import asynccontextmanager
import hashlib
import json
from pathlib import Path
import signal
import ssl
import subprocess
import sys
import time
import uuid
import wave

from numeric_pairing import NumericPairing,NumericGateway,PairingError,parse_command,write_numeric_once,HOST,PORT,DEVICE
from pairing_receipt import decode_receipt,READY,REJECTED,EXPIRED

ROOT=Path('/Users/mikedemott/Developer/AgentWork/StackChan')
CERT_HASH='7950b01015dc2f80338ec4f35581039213539eac0e6e67f4cadd0207232f3411'
CERT_BEGIN=1791540049;CERT_END=1791626449
STATES={'idle','opening','usb_ready','waiting_robot','comparing','waiting_confirmation','staging','paired','cancelled','expired','failed','closed'}

class NumericWorker:
    def __init__(self,*,approved=False,paused,serial_factory,runtime_loader,emit,clock=time.monotonic,sleep=asyncio.sleep):
        self.approved=approved;self.paused=paused;self.serial_factory=serial_factory;self.runtime_loader=runtime_loader
        self.emit=emit;self.clock=clock;self.sleep=sleep
        self.state='idle';self.port=None;self.pairing=None;self.gateway=None;self.task=None;self._closed=False
        self._deadline=None;self._usb_buffer=b'';self.cleanup_blocked=False;self._terminal_status=None
    def status(self,state,message=None):
        self.state=state;value={'state':state}
        if message:value['message']=message
        self._publish(value)
    def _publish(self,value):
        self.state=value['state']
        if value['state'] in ('failed','expired','cancelled','closed') and (self.port is not None or self.gateway is not None or self.cleanup_blocked):
            self._terminal_status=value
        else:self.emit(value)
    def pairing_status(self,value):
        self._publish(value)
    async def command(self,command):
        if command not in ('prepare','confirm','cancel','retry'):raise PairingError('invalid_command')
        if command=='cancel':
            if self.pairing:self.pairing.cancel()
            if self.task and not self.task.done():self.task.cancel();await asyncio.gather(self.task,return_exceptions=True)
            if not self.cleanup_blocked:self.status('cancelled','cancelled')
            return
        if command=='confirm':
            if not self.pairing:raise PairingError('invalid_state')
            self.pairing.mac_confirm();return
        if self._closed or self.cleanup_blocked:raise PairingError('closed')
        allowed=('idle',) if command=='prepare' else ('failed','cancelled','expired','closed')
        if self.state not in allowed or (self.task and not self.task.done()):raise PairingError('invalid_state')
        self.task=asyncio.create_task(self._prepare())
    async def wait(self):
        if self.task:await asyncio.gather(self.task,return_exceptions=True)
    def _read(self):
        try:value=self.port.read(64)
        except Exception:raise PairingError('transport_lost') from None
        if type(value) is not bytes or len(value)>64:raise PairingError('invalid_transport')
        return value
    async def _drain(self):
        until=self.clock()+2;count=0
        while self.clock()<until:
            count+=len(self._read())
            if count>4096:raise PairingError('input_limit')
            await self.sleep(.02)
    async def _receipt(self):
        until=min(self._deadline,self.clock()+120);count=0
        while self.clock()<until:
            start=self._usb_buffer.find(b'SCPS')
            if start<0:self._usb_buffer=self._usb_buffer[-3:]
            else:
                self._usb_buffer=self._usb_buffer[start:]
                if len(self._usb_buffer)>=22:
                    frame,self._usb_buffer=self._usb_buffer[:22],self._usb_buffer[22:]
                    try:return decode_receipt(frame)
                    except ValueError:raise PairingError('invalid_receipt') from None
            chunk=self._read();count+=len(chunk);self._usb_buffer+=chunk
            if count>4096:raise PairingError('input_limit')
            await self.sleep(.02)
        raise PairingError('ready_missing')
    async def _prepare(self):
        try:
            if self.approved is not True:raise PairingError('approval_required')
            if self.paused():raise PairingError('paused')
            self._deadline=self.clock()+180;self.pairing=None;self.gateway=None;self._usb_buffer=b'';self._terminal_status=None
            self.status('opening','usb_open_may_reset')
            self.port=self.serial_factory()  # Configured closed, DTR/RTS false.
            try:self.port.open()
            except Exception:raise PairingError('transport_lost') from None
            await self._drain()
            self.status('usb_ready','verify_robot_then_physical_pair')
            self.status('waiting_robot','physical_pair_required')
            ready=await self._receipt()
            if ready.status==REJECTED:raise PairingError('rejected')
            if ready.status==EXPIRED:raise PairingError('expired')
            if ready.status!=READY:raise PairingError('unsupported_receipt')
            # Runtime verification/key loading and listener happen only AFTER Ready.
            if self.paused():raise PairingError('paused')
            runtime=self.runtime_loader()
            self.pairing=NumericPairing(issuer=runtime.issue,revoke=runtime.revoke,emit=self.pairing_status,clock=self.clock)
            self.pairing.prepare(uuid.uuid4().hex,ready.boot_nonce)
            self.gateway=NumericGateway(self.pairing,runtime.transport)
            async with runtime.listener(self.gateway):
                write_numeric_once(self.port,self.pairing.numeric_frame())
                while self.clock()<self._deadline:
                    self.pairing.tick()
                    if self.pairing.state in ('failed','cancelled','expired','closed'):break
                    # A new Ready indicates reset/window replacement. Never retry.
                    data=self._read();self._usb_buffer+=data
                    marker=self._usb_buffer.find(b'SCPS')
                    if marker>=0 and len(self._usb_buffer)-marker>=22:
                        seen=decode_receipt(self._usb_buffer[marker:marker+22])
                        if seen.status==READY:raise PairingError('reset_detected' if seen.boot_nonce!=ready.boot_nonce else 'window_changed')
                        if seen.status==REJECTED:raise PairingError('rejected')
                        if seen.status==EXPIRED:raise PairingError('expired')
                        raise PairingError('unexpected_receipt')
                    if len(self._usb_buffer)>4096:raise PairingError('input_limit')
                    await self.sleep(.05)
                else:raise PairingError('expired')
        except asyncio.CancelledError:
            if self.pairing:self.pairing.cancel()
            raise
        except PairingError as failure:
            state='expired' if str(failure)=='expired' else 'failed'
            if self.pairing:self.pairing.cancel(state,str(failure))
            else:self.status(state,str(failure))
        except Exception:
            if self.pairing:self.pairing.cancel('failed','setup_or_transport_failed')
            else:self.status('failed','setup_or_transport_failed')
        finally:
            try:
                if self.gateway:await self.gateway.close()
            except Exception:
                self.cleanup_blocked=True
            finally:
                self.gateway=None
                if self.pairing and self.pairing.revocation_failed:self.cleanup_blocked=True
                if self.port is not None:
                    try:
                        self.port.close()
                        if self.port.is_open is not False:self.cleanup_blocked=True
                        else:self.port=None
                    except Exception:self.cleanup_blocked=True
                if not self.cleanup_blocked and self._terminal_status is not None:
                    value=self._terminal_status;self._terminal_status=None
                    self.state=value['state'];self.emit(value)
    async def close(self):
        self._closed=True
        if self.task and not self.task.done():self.task.cancel()
        await self.wait()
        if not self.cleanup_blocked:
            if self.pairing:self.pairing.cancel('closed','closed')
            self.status('closed','closed')

class _Runtime:
    def __init__(self,context,registry,transport):self.context=context;self.registry=registry;self.transport=transport
    def issue(self,ttl):
        # DeviceRegistry remains in memory; no durable grant database is supplied.
        _,token=self.registry.redeem_pairing_code(self.registry.create_pairing_code(),device_id=DEVICE,label='StackChan numeric transport test',capabilities=('microphone','speaker'))
        return token
    def revoke(self):self.registry.revoke(DEVICE)
    @asynccontextmanager
    async def listener(self,gateway):
        from websockets.asyncio.server import serve
        async with asyncio.timeout(120):
            async with serve(gateway.handle,HOST,PORT,ssl=self.context,process_request=gateway.process_request,max_size=4112,max_queue=2,compression=None,ping_interval=None,open_timeout=2,close_timeout=.25,server_header=None):
                yield

def _serial_factory(port):
    library=ROOT/'.tools/espressif/python_env/idf5.5_py3.13_env/lib/python3.13/site-packages'
    if str(library) not in sys.path:sys.path.insert(0,str(library))
    import serial
    connection=serial.Serial(port=None,baudrate=115200,timeout=0,write_timeout=2)
    connection.dtr=False;connection.rts=False;connection.port=port
    return connection

def _runtime(args):
    # Public certificate hash/date/IP checks precede any private key loading.
    cert=ROOT/'.tools/local-companion-tls/server.pem';key=cert.with_name('server-key.pem')
    cue=ROOT/'.tools/native-call-preparation/transport-speaker-cue.wav'
    if Path(args.cert).resolve()!=cert or Path(args.key).resolve()!=key or Path(args.cue).resolve()!=cue:raise PairingError('unpinned_files')
    if not CERT_BEGIN<=time.time()<CERT_END-130:raise PairingError('certificate_expired')
    if cert.stat().st_size>8192 or hashlib.sha256(cert.read_bytes()).hexdigest()!=CERT_HASH:raise PairingError('certificate_changed')
    if subprocess.check_output(['/usr/sbin/ipconfig','getifaddr','en1'],timeout=2,text=True).strip()!=HOST:raise PairingError('lan_unavailable')
    context=ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER);context.minimum_version=ssl.TLSVersion.TLSv1_2;context.load_cert_chain(cert,key)
    if cue.stat().st_size>600000 or hashlib.sha256(cue.read_bytes()).hexdigest()!='8c70a511b8e9dc570c277e3286343ed52077315158485a106c494557364d22b9':raise PairingError('cue_changed')
    with wave.open(str(cue),'rb') as source:
        if (source.getnchannels(),source.getsampwidth(),source.getframerate())!=(1,2,24000) or not 1<=source.getnframes()<=240000:raise PairingError('invalid_cue')
        pcm=source.readframes(source.getnframes())
    for path in (ROOT/'.tools/bridge-python',Path('/Users/mikedemott/Homelab/src')):
        if str(path) not in sys.path:sys.path.insert(0,str(path))
    from home_lab.devices import DeviceRegistry
    from home_lab.sessions import ConversationSessions
    from transport_test import TransportGateway
    registry=DeviceRegistry();transport=TransportGateway(registry,ConversationSessions(),pcm,transport_approved=True)
    return _Runtime(context,registry,transport)

def _output(value):
    # Only fixed state/message labels, comparison number and boolean/counts.
    print(json.dumps(value,separators=(',',':')),flush=True)

async def _run(args):
    worker=NumericWorker(approved=args.approved_test,paused=lambda:(ROOT/'.tools/TRANSPORT-PAIRING-PAUSED').exists(),serial_factory=lambda:_serial_factory(args.port),runtime_loader=lambda:_runtime(args),emit=_output)
    worker.status('idle')
    loop=asyncio.get_running_loop();reader=asyncio.StreamReader(limit=2048);protocol=asyncio.StreamReaderProtocol(reader)
    pipe=None;monitor=None
    try:
        pipe,_=await loop.connect_read_pipe(lambda:protocol,sys.stdin)
        def terminate():
            if pipe:pipe.close()
        for sig in (signal.SIGTERM,signal.SIGINT):loop.add_signal_handler(sig,terminate)
        async def watch_cleanup():
            while True:
                if worker.cleanup_blocked:
                    terminate();return
                await asyncio.sleep(.05)
        monitor=asyncio.create_task(watch_cleanup())
        while True:
            line=await reader.readline()
            if not line:break
            try:await worker.command(parse_command(line.decode('utf-8')))
            except Exception:worker.status(worker.state,'invalid_command')
    except Exception:worker.status('failed','input_failed')
    finally:
        if monitor:
            monitor.cancel();await asyncio.gather(monitor,return_exceptions=True)
        await worker.close()
        if pipe:pipe.close()
        for sig in (signal.SIGTERM,signal.SIGINT):loop.remove_signal_handler(sig)

def main(argv=None):
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--approved-test',action='store_true')
    parser.add_argument('--port',default='/dev/cu.usbmodem31201')
    parser.add_argument('--cert',default=str(ROOT/'.tools/local-companion-tls/server.pem'))
    parser.add_argument('--key',default=str(ROOT/'.tools/local-companion-tls/server-key.pem'))
    parser.add_argument('--cue',default=str(ROOT/'.tools/native-call-preparation/transport-speaker-cue.wav'))
    args=parser.parse_args(argv)
    try:asyncio.run(_run(args))
    except BaseException:_output({'state':'closed','message':'closed'})
    return 0
if __name__=='__main__':raise SystemExit(main())
