"""Inert latest-only WCV1 -> existing Swift worker-envelope adapter. No IO.

All owner/clock callbacks are required injected contracts, not supplied authority.
No production caller is connected. Control/audio-drained flags are scheduling
assertions only; they cannot grant purpose, capture provenance or Stop evidence.
Caller serializes access and handles retained physical cleanup separately.
"""
import base64
import json
import math

from webcam_transport import (
    FRAME_BYTES, MAX_U64, MappedCapture, VideoFrame, WebcamPurpose,
    encode_fragments,
)


class BoundedWebcamWorkerEmitter:
    """One frame reference + one fragment per pull; replacement drops old video.

    This adapter does not establish a live throughput/freshness qualification.
    In particular one pull per 20ms tick cannot finish a full frame within 250ms.
    The existing scheduler must provide a separately qualified bounded budget.
    """
    def __init__(self, purpose, *, current, map_capture_clock, clock):
        if type(purpose) is not WebcamPurpose:
            raise ValueError('exact existing webcam purpose required')
        if not all(callable(f) for f in (current, map_capture_clock, clock)):
            raise ValueError('explicit owner and capture clock contracts required')
        self._purpose = purpose
        self._current = current
        self._map_capture = map_capture_clock
        self._clock = clock
        self._last_clock = -1
        self._last_sequence = 0
        self._pending = None
        self._fragments = None
        self._capture_ns = None
        self._revoked = False

    def revoke(self):
        """Discard video only. Does not acknowledge physical camera shutdown."""
        self._revoked = True
        self._pending = self._fragments = self._capture_ns = None

    def _checked(self, frame, captured_ns):
        if self._revoked:
            raise PermissionError('emitter revoked')
        now = self._clock()
        if (type(now) not in (int, float) or not math.isfinite(now)
                or now < 0 or now < self._last_clock):
            raise PermissionError('monotonic clock required')
        self._last_clock = now
        if self._current(self._purpose) is not True:
            raise PermissionError('current exact webcam purpose required')
        mapped = self._map_capture(self._purpose, captured_ns)
        if (type(mapped) is not MappedCapture
                or type(mapped.host_time) not in (int, float)
                or type(mapped.uncertainty) not in (int, float)
                or not math.isfinite(mapped.host_time)
                or not math.isfinite(mapped.uncertainty)
                or not 0 <= mapped.uncertainty <= .025
                or mapped.host_time != frame.host_captured
                or mapped.host_time < 0 or mapped.host_time > now
                or now - mapped.host_time + mapped.uncertainty > .25):
            raise PermissionError('trusted fresh capture mapping required')
        # Callbacks may synchronously invalidate this serialized adapter.
        if self._revoked or self._current(self._purpose) is not True:
            raise PermissionError('camera purpose lost during validation')

    def offer(self, frame, *, producer_capture_ns):
        """Accept a current typed assembler frame plus its original producer time.

        The caller must retain original capture metadata; arrival time is not a
        substitute. A later frame replaces even a partially drained older one.
        Any rejection revokes this adapter; a new purpose requires a new adapter.
        """
        try:
            if (type(frame) is not VideoFrame or frame.purpose != self._purpose
                    or type(frame.sequence) is not int
                    or not self._last_sequence < frame.sequence <= MAX_U64
                    or type(frame.bgra) is not bytes or len(frame.bgra) != FRAME_BYTES
                    or type(frame.host_captured) not in (int, float)
                    or not math.isfinite(frame.host_captured)
                    or type(producer_capture_ns) is not int
                    or not 1 <= producer_capture_ns <= MAX_U64):
                raise ValueError('bounded current typed frame required')
            self._checked(frame, producer_capture_ns)
            self._last_sequence = frame.sequence
            self._pending = frame
            self._capture_ns = producer_capture_ns
            self._fragments = encode_fragments(
                self._purpose, frame.sequence, producer_capture_ns, frame.bgra)
        except BaseException:
            self.revoke()
            raise

    def next_event(self, *, controls_drained, audio_drained):
        """Return one compact JSON payload (no newline) or None. Never writes it.

        Caller may append one newline to the existing pipe. Both scheduling flags
        must be exactly True; video never drains ahead of pending control/audio.
        No queue of encoded messages is retained and no sink callback executes.
        """
        if self._revoked or self._pending is None:
            return None
        try:
            # Revalidate even while backpressured so authority cannot linger.
            self._checked(self._pending, self._capture_ns)
            if controls_drained is not True or audio_drained is not True:
                return None
            wire = next(self._fragments)
            encoded = base64.b64encode(wire).decode('ascii')
            if len(wire) > 4112 or len(encoded) > 5484:
                raise ValueError('worker fragment bound exceeded')
            event = json.dumps({'type': 'webcam_fragment', 'wire': encoded},
                               separators=(',', ':'), allow_nan=False).encode('ascii')
            if len(event) + 1 > 8192:
                raise ValueError('worker line bound exceeded')
            return event
        except StopIteration:
            self._pending = self._fragments = self._capture_ns = None
            return None
        except BaseException:
            self.revoke()
            raise
