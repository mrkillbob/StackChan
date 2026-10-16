# Mira Companion shell lifecycle (todo20)

Mira home is the launch surface for every trust state. It renders the approved Mira frame, bundled unchanged. Pairing starts only from its explicit button. Production has no durable trust store: `UnavailableLaunchTrust` reports absent and `RememberedPairing.mayAuthorizeSession` remains false. One-time remembered pairing is still a coverage gap.

A worker report of `.paired` returns to Mira home, which says “Paired for this session,” retains the worker instruction, countdown and Stop session control, and disables starting another pairing. Restoring or unconfirmed cleanup remains visible. No session pairing survives app restart or grants call authority.

Production uses `UnsupportedSameCallRoute`. No supported start/join/hangup/raw-audio attachment to the existing Mira call has been established. The UI does not render “In call” from pairing or WSS/PCM readiness. Local `native_session_ref` is bookkeeping. The prior physical trial consumed 1,457,808 PCM frames but delivered zero speaker packets; same-call continuity and speaker playback remain unverified.

The inert future-route lifecycle starts muted and requires a current-epoch connection report, existing robot physical consent, and explicit unmute before its capture gate can arm. Mute disables speaker; unmute does not re-enable it. Those fixtures do not connect a production call or control production audio.

Quit invalidates the epoch and disarms local lifecycle state, then invokes the worker’s atomic `PairingController.shutdown`. Release is confirmed only for that exact worker exit. Hangup is requested only if a future supported route advertises it. Failure refuses application termination and reports cleanup incomplete. Existing physical stop and worker Cancel controls remain in place.

`script/test_offline.sh` covers decoding, real-pipe worker cleanup, consent/session behavior, remembered-device rules and inert Mira lifecycle/presentation cases. Set `STACKCHAN_OFFLINE_BUILD_ROOT` and `STACKCHAN_UI_BUILD_ROOT` to `/tmp` paths. Build only with `--build-only`; no app launch or hardware acceptance is part of this checkpoint.
