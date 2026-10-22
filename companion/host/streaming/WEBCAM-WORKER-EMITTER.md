# Inactive worker fragment emitter

`webcam_worker_emitter.BoundedWebcamWorkerEmitter` is a pure adapter; no production
caller imports it. Existing webcam unavailable defaults remain unchanged. It
creates no owner, permission, clock calibration, physical Stop receipt, IO or CMIO
readiness.

Construct with an existing `WebcamPurpose` and required injected callbacks:
`current(purpose) -> exactly True`,
`map_capture_clock(purpose, original_producer_nanoseconds) -> MappedCapture`, and
`clock() -> monotonic seconds`. These are trusted caller contracts, not a way to
qualify a physical producer. Offer the existing typed `VideoFrame` together with
its original producer timestamp. `VideoFrame` currently omits that timestamp;
a future delivery caller must preserve it from original authenticated metadata,
never infer it from packet arrival or mapped host seconds.

`offer(frame, producer_capture_ns=...)` retains one latest frame. A newer sequence
replaces even a partially emitted old frame. `next_event(controls_drained=True,
audio_drained=True)` returns at most one compact JSON payload, without newline;
the future caller may append one newline to its existing worker pipe. The object
contains exactly `type: webcam_fragment` and base64 `wire`, matching Swift
`CompanionWebcamPipeline.handleExistingWorkerEvent`. Wire bytes reuse the existing
WCV1 encoder and stay <=4112; base64 <=5484; payload plus newline <=8192. Both
scheduling flags must be exactly True and never establish camera authority.
Rejections revoke/drop all pending video. `revoke()` does not confirm physical Off
or release the caller's retained Stop cleanup custody. Caller serializes access.

A 307200-byte frame needs 76–83 fragments depending on identifier lengths. One
fragment per 20ms audio tick takes at least 1.52 seconds, exceeding the 250ms
capture freshness limit. The synthetic fixed-clock roundtrip is wire/schema
coverage only. A real scheduler needs separately reviewed bounded delivery and
backpressure behavior; this patch does not supply or qualify that scheduler.
The increasing-clock test intentionally demonstrates rejection before a complete
frame at a 20ms-per-fragment schedule. No frame rate, camera timing, CMIO or live
end-to-end acceptance is claimed.

`test_webcam_worker_emitter.py` uses only synthetic bytes, injected callbacks,
JSON/base64 and the existing pure `WebcamAssembler`. It neither imports the worker
runtime nor starts any process, socket, native library, camera or model. Root
runs it serially under the existing audit/guard. Physical camera/extension/runtime
activation remains a separate agreed scope.
