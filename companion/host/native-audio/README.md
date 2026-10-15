# Native audio implementation slice

Architecture: robot audio → existing Opus/resampler worker → native AUHAL output on **Pro Tools Audio Bridge 2-A_UID** → the existing Mira native Voice input. Mira output → **Pro Tools Audio Bridge 2-B_UID** → native AUHAL input → existing resampler/Opus worker → robot speaker. No model/session API is substituted. Native Voice remains the conversation owner; the bridge owns transport epochs only.

`coreaudio_bridge.cpp` implements the two public AUHAL instances described by [Apple TN2091](https://developer.apple.com/library/archive/technotes/tn2091/_index.html). Only those two exact inventoried virtual UIDs are accepted. Client format is mono signed PCM16 48kHz on physical channel one, other output channels disabled; physical rate must already be 48kHz. No default device, device rate, permissions or persistent settings are written. Library load performs no IO. `sc_open` and `sc_start` are explicit physical-test operations, not invoked by synthetic tests.

Callbacks use preallocated storage, lock-free atomics and one-producer/one-consumer rings bounded at 7680 samples (160ms). Overflow drops new samples, with counters, rather than racing a consumer cursor to drop old samples. Underflow is zero-filled. Epoch-tagged entries reject stale in-flight callbacks without resetting shared cursors. `sc_mute` prevents rearming until a new epoch begins; playback invalidates queued robot capture and suppresses capture forwarding. These local gates do not cancel the remote native Voice response or flush a physical robot speaker. One worker owns controls and PCM APIs; Python never runs inside callbacks.

`native_adapter.py` provides a worker-side adapter for the existing `CodecHostBridge`. Its `open_for_approved_test` / `arm_for_approved_test` are explicit calls. The native C API chunks are <=2048 samples; the binding splits longer bounded resampler output before submitting. Control lifecycle is begin(new epoch), arm, PCM, playing status; interrupt invalidates codec pending data and mutes, then a new begin/handshake is required. Device removal and callback errors fail muted; a finite external supervisor must stop/close.

`audio_session.hpp` is the custom firmware's serialized control gate: acknowledged monotonic epoch, physical consent before arm, independent per-direction sequence/replay checking, bounded Opus packets, half duplex, heartbeat expiry and interruption. It performs no hardware/network/NVS action. Codec task/transport and actual speaker-stop acknowledgement are subsequent wiring work; this header alone is not firmware streaming.

New tests exercise actual native render callbacks with stack-owned buffers, playback queue invalidation, mute/rearm rejection and callback-error gating, as well as wraparound/overflow, zero underflow, stale epoch filtering, 100000 concurrent ordered samples, and firmware handshake/consent/replay/interruption/heartbeat expiry. Prior codec and host-adapter tests are retained, not replayed. Compile and tests never call `sc_open`, `sc_start`, microphone, network, or provider APIs.

Build, with the existing Xcode compiler, into an owned temporary output:

```sh
STACKCHAN_SDK="$(xcrun --sdk macosx --show-sdk-path)"
xcrun clang++ -isysroot "$STACKCHAN_SDK" -isystem "$STACKCHAN_SDK/usr/include/c++/v1" -std=c++17 -Wall -Wextra -Werror -dynamiclib coreaudio_bridge.cpp -framework AudioUnit -framework CoreAudio -framework CoreFoundation -o libstackchan_audio.dylib
xcrun clang++ -isysroot "$STACKCHAN_SDK" -isystem "$STACKCHAN_SDK/usr/include/c++/v1" -std=c++17 -Wall -Wextra -Werror test_ring.cpp -o test_ring
xcrun clang++ -isysroot "$STACKCHAN_SDK" -isystem "$STACKCHAN_SDK/usr/include/c++/v1" -std=c++17 -Wall -Wextra -Werror test_session.cpp -o test_session
```

No flash command is supplied. Baseline shipped firmware/NVS recovery must be qualified independently of this development slice.

Run `python3 -B verify.py` for the finite synthetic verification and captured compiler/test receipts. The supervisor deletes only its owned temporary outputs; the compiled library hash is retained in VERIFICATION.json.
