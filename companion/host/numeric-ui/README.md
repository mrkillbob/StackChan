# Local StackChan matching-number window

USB and verified Wi-Fi TLS carry pairing; there is no Bluetooth transport.
Click Pair StackChan, then physically tap Pair trusted host on the muted robot.
Compare the six digits and confirm on both devices. Cancel/Retry closes the attempt.
Pairing never automatically invokes Connect or Allow. Those remain separate robot actions.
Audio is blocked by default. A separately approved local audio check requires both
`--approved-test` and `--approved-audio`, a matching fresh one-use authorization,
and both matching-number confirmations before any audio admission.

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
