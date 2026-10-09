"""Bounded remote connection driver; no listener, TLS setup, pairing or provider.
Caller supplies an already authenticated StreamingSession and accepted socket.
This driver works without CoreAudio or a Mac; it does not establish Mira identity.
"""
import asyncio
import json
import time
from streaming_session import parse_control


async def run_connection(socket, session, *, duration=900.0, send_timeout=0.25):
    """Own one receive task, service 20 ms audio ticks, and fail closed on exit.
    Existing session authority/watchdog is checked even when the peer is silent.
    Blocking codec/provider work must be bounded by the supplied adapter.
    """
    if type(duration) not in (int, float) or not 0 < duration <= 900:
        raise ValueError('bounded connection duration required')
    if type(send_timeout) not in (int, float) or not 0 < send_timeout <= 0.25:
        raise ValueError('bounded send deadline required')
    receive = None
    deadline = time.monotonic() + duration
    next_tick = time.monotonic()
    playback = None
    try:
        receive = asyncio.create_task(socket.recv())
        while True:
            now = time.monotonic()
            if now >= deadline:
                raise TimeoutError('connection duration expired')
            session._authorized()
            ready, _ = await asyncio.wait({receive}, timeout=min(max(0, next_tick-now), deadline-now))
            if ready:
                message = receive.result()
                if type(message) is str:
                    answer = session.control(parse_control(message))
                    response = json.dumps(answer, separators=(',', ':'), allow_nan=False)
                    if len(response.encode()) > 1024:
                        raise ValueError('bounded control response required')
                    await asyncio.wait_for(socket.send(response), send_timeout)
                elif type(message) is bytes and 17 <= len(message) <= 4112:
                    session.receive(message)
                else:
                    raise ValueError('bounded audio/control message required')
                receive = asyncio.create_task(socket.recv())
            now = time.monotonic()
            if now >= next_tick:
                if session.armed:
                    speaking = session.bridge.adapter.playing
                    if type(speaking) is not bool:
                        raise ValueError('explicit provider playback state required')
                    state = (session.epoch, speaking)
                    if state != playback:
                        response = json.dumps(session.playing(speaking), separators=(',', ':'), allow_nan=False)
                        await asyncio.wait_for(socket.send(response), send_timeout)
                        playback = state
                    if not speaking:
                        next_tick = now + 0.02
                        continue
                    # 960 mono48k samples = 20 ms. Codec output is bounded per tick.
                    packets = session.outgoing(960)
                    if type(packets) is not list or len(packets) > 2:
                        raise BufferError('outgoing packet bound exceeded')
                    for packet in packets:
                        if type(packet) is not bytes or not 17 <= len(packet) <= 4112:
                            raise ValueError('bounded outgoing audio required')
                        await asyncio.wait_for(socket.send(packet), send_timeout)
                next_tick = now + 0.02
    finally:
        # Invalidate audio before awaiting socket/task teardown.
        try:
            session.close()
        finally:
            if receive is not None:
                receive.cancel()
                await asyncio.gather(receive, return_exceptions=True)
            await asyncio.wait_for(socket.close(), send_timeout)
