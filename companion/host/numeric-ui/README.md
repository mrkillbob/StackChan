# StackChan Companion

The visible flow includes manual original-call readiness, matching-number pairing,
accepted physical microphone consent, bounded conversation and verified restoration.
See [offline boundaries and next-trial gates](COMPANION-OFFLINE.md) and
[HomeLab/Apple status and draft seams](HOME-INTEGRATION.md). Remember/Forget is an
unsaved preference preview; durable enrollment and integration execution are disabled.

USB and verified Wi-Fi TLS carry pairing; there is no Bluetooth transport.
Click Pair StackChan, then physically tap Pair on the robot.
Compare the six digits and confirm on both devices. Cancel/Retry closes the attempt.
On installed a67e6c2, an audio-scoped matching confirmation is explicitly labeled
"Pair & authorize mic/speaker" and gives physical consent for that one local session.
Companion has no microphone-only mute: leave physical Listen off when audio is not
authorized, or stop the whole audio session. A stop reports release only after a clean
worker exit; a call in the user's call app may continue. The robot physical stop also ends
the session.
Audio is blocked by default. A separately approved local audio check requires both
`--approved-test` and `--approved-audio`, a matching fresh one-use authorization,
a valid `expected_boot_nonce` in that fresh authorization, and both matching-number
confirmations before any audio admission. A mismatched Ready aborts before key
loading, listener creation, protocol write or grant; its nonce/reset evidence survives.
The pairing backend suppresses application DTR/RTS updates and clears/verifies HUPCL
while retaining one bounded SCNP write. OS USB behavior still requires physical
boot continuity qualification; these controls cannot prove reset-free hardware.

Default app launch is an inert preview. Live mode requires a separately approved
exact firmware image and test, `--approved-test`, and removal of the project pause
marker by the approved test operator. Keep the marker until that approval.
The window has no token field, clipboard operation, or credential log.

```sh
companion/host/numeric-ui/script/build_and_run.sh --build-only
companion/host/numeric-ui/script/test_offline.sh
# Preview only:
companion/host/numeric-ui/script/build_and_run.sh run
```

Build output is in the dedicated project `.tools/numeric-ui/StackChanPairing.app`.
Use `STACKCHAN_UI_BUILD_ROOT` for an isolated build-only output and
`STACKCHAN_OFFLINE_BUILD_ROOT` for isolated fixture binaries/module cache.
Coordinate finite offline checks with the existing local CI resource owner before running them.
The script uses installed Xcode Swift; it installs no dependencies and starts no
GitHub Actions. Offline tests use owned synthetic subprocesses without USB or LAN
listeners. The worker anchors its deadline at robot Ready and uses a 118-second budget,
including runtime loading and a two-second USB/control-loop margin. Earlier
unexpected disconnects still fail; elapsed local deadlines report expiry. The
margin does not guarantee alignment under arbitrarily delayed USB delivery.
The real worker uses the existing project-local Python bridge packages,
the pinned Mac endpoint, approved certificate and cue, and a memory-only grant.
The prepared TLS certificate expires October 10, 2026 at 10:00:49 UTC; live mode
fails closed near expiry. A replacement trust profile needs separate approval.

The comparison code is a human session check within previously approved TLS trust;
it is not a password or a generic Bluetooth-style cryptographic SAS protocol.
Physical robot confirmation is required because the host does not independently
authenticate a client certificate for the robot.
