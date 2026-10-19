"""Bounded remote connection driver; no listener, TLS setup, pairing or provider.
Caller supplies an already authenticated StreamingSession and accepted socket.
This driver works without CoreAudio or a Mac; it does not establish Mira identity.
"""
import asyncio
import json
import time
from streaming_session import parse_control
from transport_diagnostics import TransportDiagnostics


class TransportComplete(Exception):
    """Expected finite audio/conversation completion; cleanup remains diagnosed."""


class OwnedSocketClose:
    """One exact close operation; timeout/cancellation never abandons its task."""
    def __init__(self,socket):self.socket=socket;self.task=None;self.confirmed=False
    async def close(self,timeout=.25,**kwargs):
        if self.confirmed:return
        if self.task is None:self.task=asyncio.create_task(self.socket.close(**kwargs))
        done,_=await asyncio.wait({self.task},timeout=timeout)
        if not done:raise TimeoutError('socket cleanup pending')
        task=self.task
        try:task.result()
        except BaseException:self.task=None;raise
        self.confirmed=True


async def run_connection(socket, session, *, duration=900.0, send_timeout=0.25, diagnostics=None, retain_cleanup=None, socket_owner=None):
    """Own one receive task, service 20 ms audio ticks, and fail closed on exit.
    Existing session authority/watchdog is checked even when the peer is silent.
    Blocking codec/provider work must be bounded by the supplied adapter.
    """
    if duration is not None and (type(duration) not in (int, float) or not 0 < duration <= 900):
        raise ValueError('bounded connection duration required')
    if type(send_timeout) not in (int, float) or not 0 < send_timeout <= 0.25:
        raise ValueError('bounded send deadline required')
    receive = None
    socket_owner=socket_owner or OwnedSocketClose(socket)
    diagnostics = diagnostics or TransportDiagnostics(getattr(session,'clock',time.monotonic))
    diagnostics.observe(session)
    primary = None
    deadline = float("inf") if duration is None else time.monotonic() + duration
    next_tick = time.monotonic()
    playback = None
    async def send_control(answer):
        response=json.dumps(answer,separators=(',',':'),allow_nan=False)
        if len(response.encode())>1024:raise ValueError('bounded control response required')
        diagnostics.set_phase('control_send')
        await asyncio.wait_for(socket.send(response),send_timeout)
    try:
        receive = asyncio.create_task(socket.recv())
        while True:
            now = time.monotonic()
            if now >= deadline:
                diagnostics.set_phase('duration')
                raise TimeoutError('connection duration expired')
            diagnostics.set_phase('authority')
            diagnostics.observe(session)
            session._authorized()
            ready, _ = await asyncio.wait({receive}, timeout=min(max(0, next_tick-now), deadline-now))
            if ready:
                diagnostics.set_phase('receive')
                message = receive.result()
                if type(message) is str:
                    diagnostics.set_phase('control')
                    control = parse_control(message)
                    diagnostics.control_label(control.get('type'))
                    start=getattr(session,'start_control',session.control)
                    answer=start(control)
                    if answer is not None:
                        diagnostics.accepted_control(session,control.get('type'))
                        await send_control(answer)
                elif type(message) is bytes and 17 <= len(message) <= 4112:
                    diagnostics.set_phase('audio_receive')
                    session.receive(message)
                else:
                    raise ValueError('bounded audio/control message required')
                receive = asyncio.create_task(socket.recv())
            poll=getattr(session,'poll_control',None)
            if callable(poll):
                diagnostics.set_phase('native_startup')
                answer=poll()
                if answer is not None:
                    diagnostics.accepted_control(session,'consent')
                    await send_control(answer)
            now = time.monotonic()
            if now >= next_tick:
                if session.armed:
                    diagnostics.set_phase('playback_state')
                    packets = None
                    pump = getattr(session, 'audio_tick', None)
                    if callable(pump):
                        speaking, packets = pump(960)
                    else:
                        speaking = session.bridge.adapter.playing
                    if type(speaking) is not bool:
                        raise ValueError('explicit provider playback state required')
                    state = (session.epoch, speaking)
                    if state != playback:
                        diagnostics.set_phase('playback_state')
                        response = json.dumps(session.playing(speaking), separators=(',', ':'), allow_nan=False)
                        diagnostics.set_phase('playback_send')
                        await asyncio.wait_for(socket.send(response), send_timeout)
                        playback = state
                    if not speaking:
                        next_tick = now + 0.02
                        continue
                    # 960 mono48k samples = 20 ms. Codec output is bounded per tick.
                    diagnostics.set_phase('audio_pull')
                    if packets is None:packets = session.outgoing(960)
                    if type(packets) is not list or len(packets) > 2:
                        raise BufferError('outgoing packet bound exceeded')
                    for packet in packets:
                        if type(packet) is not bytes or not 17 <= len(packet) <= 4112:
                            raise ValueError('bounded outgoing audio required')
                        diagnostics.set_phase('audio_send')
                        await asyncio.wait_for(socket.send(packet), send_timeout)
                        diagnostics.speaker_sent()
                next_tick = now + 0.02
    except BaseException as failure:
        primary = failure
        diagnostics.observe(session)
        diagnostics.fault(failure)
        raise
    finally:
        # Invalidate authority first; cleanup never replaces the original fault.
        cleanup = None
        try:
            session.close()
        except BaseException as failure:
            diagnostics.fault(failure,'session_cleanup',cleanup=True)
            cleanup = failure
            if retain_cleanup is not None:retain_cleanup('session',session)
        if receive is not None:
            receive.cancel()
            try:
                await asyncio.gather(receive, return_exceptions=True)
            except BaseException as failure:
                diagnostics.fault(failure,'receive_cleanup',cleanup=True)
                if cleanup is None:cleanup = failure
        try:
            await socket_owner.close(timeout=send_timeout)
        except BaseException as failure:
            diagnostics.fault(failure,'socket_cleanup',cleanup=True)
            if cleanup is None:cleanup = failure
            if retain_cleanup is not None:retain_cleanup('socket',socket_owner)
        if primary is None and cleanup is not None:
            raise cleanup
