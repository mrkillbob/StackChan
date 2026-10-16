"""Existing native-call audio only. Import/profile validation performs no IO.
The operator confirms the original call and its two-bus route; no model, TTS,
call-start API, device-default change or inferred conversation identity exists here.
"""
from dataclasses import dataclass
from pathlib import Path
from array import array
import hashlib
import time
from host_adapter import CodecHostBridge
from remote_gateway import RemoteGateway, QualifiedProvider, ProviderCleanupError
from streaming_session import AudioBackpressure, SustainedAudioBackpressure, StreamingSession, decode_frame, encode_frame
from gateway_connection import TransportComplete
from transport_diagnostics import project_native_audio

ROOT = Path('/Users/mikedemott/Developer/AgentWork/StackChan')
LIBRARY = Path('/Users/mikedemott/Developer/AgentWork/Codex/2026-10-11/task-2/evidence/companion-reference-research/native-close-next/libstackchan_audio_checked.dylib')
LIBRARY_HASH = '858e1769fb94fa99d040db119275ccea8bb4b9de5c5ee35e5ed24a182855ea16'

@dataclass(frozen=True)
class NativeRoute:
    session_reference: str
    duration_seconds: int = 60

    @classmethod
    def from_dict(cls, value):
        if type(value) is not dict or not set(value) <= {'session_reference','call_confirmed','route_confirmed','forwarding_approved','duration_seconds'}:
            raise PermissionError('explicit existing-call route required')
        if not all(value.get(k) is True for k in ('call_confirmed','route_confirmed','forwarding_approved')):
            raise PermissionError('call, route and forwarding approval required')
        ref=value.get('session_reference');duration=value.get('duration_seconds',60)
        if type(ref) is not str or not 1<=len(ref)<=128 or any(ord(c)<32 for c in ref):
            raise ValueError('existing call reference required')
        if type(duration) is not int or not 15<=duration<=90:
            raise ValueError('bounded native conversation duration required')
        return cls(ref,duration)

class NativeVoiceBridge(CodecHostBridge):
    """Drain real return PCM every20ms; gate turn-taking without storing speech."""
    def __init__(self, adapter, decoder, encoder, up, down, *, clock=time.monotonic):
        super().__init__(adapter,decoder,encoder,up,down)
        self.clock=clock;self.voice_until=0.;self.closed=False;self._close_pending=None
        self.mic_packets=self.speaker_packets=self.return_frames=0
        self.native_audio={}

    def begin(self, epoch):
        if self.closed or self._close_pending is not None:raise PermissionError("native bridge closing")
        super().begin(epoch);self.voice_until=0.

    def receive_opus(self, epoch, packet):
        if self.closed or self._close_pending is not None:raise PermissionError("native bridge closing")
        try:
            accepted=super().receive_opus(epoch,packet)
        except AudioBackpressure:
            self.native_audio=project_native_audio(self.adapter.diagnostics())
            raise
        if accepted:self.mic_packets+=1
        else:self.native_audio=project_native_audio(self.adapter.diagnostics())
        return accepted

    def poll_voice(self, epoch, frames):
        if self.closed or self._close_pending is not None or epoch!=self.epoch or self.adapter.muted:return False,[]
        diagnostics=self.adapter.diagnostics()
        self.native_audio=project_native_audio(diagnostics)
        # Loss in the robot-to-call ring is counted and packet-scoped below;
        # loss in the call-to-robot return ring or a CoreAudio error still fails closed.
        if any(diagnostics.get(k,0) for k in ('error','input_dropped')):
            raise RuntimeError('native audio callback/queue failure')
        pull=getattr(self.adapter,'pull_pcm',None)
        if callable(pull):
            queued=diagnostics.get('input_queue_frames')
            if type(queued) is not int or not 0<=queued<=7680:
                raise ValueError('bounded native return queue required')
            # Callback clock supplies the amount to drain, not a nominal20ms timer.
            # <=120ms PCM/tick, <=3 pulls and <=2 Opus packets; never enlarge ring.
            remaining=min(queued,5760);chunks=[]
            for _ in range(3):
                if not remaining:break
                request=min(remaining,2048);data=pull(epoch,request)
                if type(data) is not bytes or len(data)%2 or len(data)>request*2:
                    raise ValueError('bounded actual native PCM return required')
                if not data:break
                chunks.append(data);remaining-=len(data)//2
            pcm=b''.join(chunks)
        else:
            # Existing inert synthetic adapter contract returns padded frames.
            pcm=self.adapter.robot_output_pcm(epoch,frames)
            if type(pcm) is not bytes or len(pcm)!=frames*2:
                raise ValueError('bounded synthetic PCM return required')
        if self.adapter.muted:raise RuntimeError('native audio return failed')
        self.return_frames+=len(pcm)//2
        samples=array('h');samples.frombytes(pcm)
        if any(abs(x)>64 for x in samples):self.voice_until=self.clock()+.30
        active=self.clock()<self.voice_until
        if self.adapter.playing!=active:
            self.adapter.set_playing(epoch,active)
        if not active:
            self.pending.clear();self.down.reset();return False,[]
        if pcm:self.pending.extend(self.down.process(pcm))
        if len(self.pending)>11520:raise BufferError('native return bound exceeded')
        packets=[]
        while len(self.pending)>=2880:
            packets.append(self.encoder.encode(bytes(self.pending[:2880])))
            del self.pending[:2880]
        self.speaker_packets+=len(packets)
        return True,packets

    def close(self):
        if self.closed:return
        first_error=None
        if self._close_pending is None:
            self._close_pending=[self.adapter.close,self.decoder.close,self.encoder.close,self.up.close,self.down.close]
            if self.epoch and not self.adapter.muted:
                try:self.interrupt(self.epoch)
                except BaseException as failure:first_error=failure
        remaining=[]
        for operation in self._close_pending:
            try:operation()
            except BaseException as failure:
                remaining.append(operation)
                if first_error is None:first_error=failure
        self._close_pending=remaining
        self.pending.clear()
        self.closed=not remaining
        if first_error is not None:raise first_error

class NativeConversationSession(StreamingSession):
    def __init__(self,*args,duration_seconds=60,**kwargs):
        super().__init__(*args,**kwargs)
        self.duration_seconds=duration_seconds;self.started_at=None
        self.on_complete=lambda:None;self.suppressed_mic_packets=0;self.backpressure_mic_packets=0
        self.backpressure_mic_streak=0
        # A mic packet is 60 ms; five consecutive queue misses bound a silent uplink to 300 ms.
        self.max_backpressure_mic_streak=5

    def _authorized(self):
        super()._authorized()
        if self.armed:
            if self.started_at is None:self.started_at=self.clock()
            if self.clock()-self.started_at>=self.duration_seconds:
                self.on_complete()
                raise TransportComplete('bounded native conversation completed')

    def receive(self,wire):
        self._authorized();epoch,sequence,speaker,payload=decode_frame(wire)
        if not self.armed or not self._active() or epoch!=self.epoch or speaker:
            raise PermissionError('native audio direction/authority')
        if sequence!=self.mic_seq:
            self.interrupt();raise ValueError('native microphone sequence gap')
        if self.bridge.adapter.playing:
            self.suppressed_mic_packets+=1;self.mic_seq+=1
            self.backpressure_mic_streak=0
            return
        try:
            super().receive(wire)
        except AudioBackpressure:
            # Drop one complete, numbered uplink packet. A short queue spike is
            # recoverable; sustained loss interrupts the call instead of leaving a
            # live-looking session with a dead microphone.
            self.backpressure_mic_packets+=1;self.mic_seq+=1
            self.backpressure_mic_streak+=1
            if self.backpressure_mic_streak>=self.max_backpressure_mic_streak:
                self.interrupt()
                raise SustainedAudioBackpressure('sustained native microphone queue backpressure')
        else:
            self.backpressure_mic_streak=0

    def audio_tick(self,frames):
        self._authorized()
        if not self.armed or not self._active():
            self.interrupt();return False,[]
        active,packets=self.bridge.poll_voice(self.epoch,frames)
        wire=[]
        for packet in packets:
            if self.speaker_seq==0xffffffff:raise OverflowError('speaker sequence exhausted')
            wire.append(encode_frame(self.epoch,self.speaker_seq,True,packet));self.speaker_seq+=1
        return active,wire

    def statistics(self):
        return dict(native_conversation=True,consent_received=self.started_at is not None,
                    microphone_packets=self.bridge.mic_packets,speaker_packets=self.bridge.speaker_packets,
                    native_return_frames=self.bridge.return_frames,
                    suppressed_mic_packets=self.suppressed_mic_packets,
                    backpressure_mic_packets=self.backpressure_mic_packets,
                    native_audio=dict(self.bridge.native_audio))


class _PartialCloseGroup:
    def __init__(self,resources):self.pending=list(resources)
    def close(self):
        remaining=[];first=None
        for resource in self.pending:
            try:resource.close()
            except BaseException as failure:
                remaining.append(resource)
                if first is None:first=failure
        self.pending=remaining
        if first is not None:raise first


def make_provider(route):
    """Called only by admitted audio socket, after on-device compare/permission."""
    if not isinstance(route,NativeRoute):raise PermissionError('confirmed native route required')
    if hashlib.sha256(LIBRARY.read_bytes()).hexdigest()!=LIBRARY_HASH:
        raise RuntimeError('reviewed native audio library changed')
    from native_adapter import NativeHostAdapter
    from codec import OpusCodec,Resampler
    adapter=NativeHostAdapter(LIBRARY);components=[];bridge=None
    try:
        for create in (lambda:OpusCodec(16000),lambda:OpusCodec(24000),lambda:Resampler(16000,48000),lambda:Resampler(48000,24000)):
            components.append(create())
        bridge=NativeVoiceBridge(adapter,*components)
        adapter.open_for_approved_test()
        return QualifiedProvider(bridge,adapter.arm_for_approved_test,bridge.close,
                                 route.session_reference,identity_verified=True,kind='existing_mira')
    except BaseException:
        owner=bridge if bridge is not None else _PartialCloseGroup([adapter,*components])
        try:owner.close()
        except BaseException:
            partial=QualifiedProvider(owner,None,owner.close,route.session_reference,identity_verified=False)
            raise ProviderCleanupError(partial) from None
        raise


def native_gateway(registry,sessions,route):
    def session_factory(*args,**kwargs):
        return NativeConversationSession(*args,duration_seconds=route.duration_seconds,**kwargs)
    return RemoteGateway(registry,sessions,lambda _:make_provider(route),session_factory=session_factory)
