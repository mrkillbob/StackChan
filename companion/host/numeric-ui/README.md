# StackChan Companion source

This source includes the manual original-call flow, matching-number pairing,
remembered enrollment/forget controls, physical audio consent, bounded sessions
and checked cleanup. The public build has empty trust pins and expiry zero;
those source capabilities are not evidence of a configured or active session.
The existing saved device pairing is separate from this source qualification.

USB and verified Wi-Fi TLS carry pairing; there is no Bluetooth transport.
The comparison code is a human session check within approved TLS trust, not a
password or a generic cryptographic SAS protocol. Both confirmations remain
required for enrollment. Camera and wake providers remain unavailable until
their separate qualification gates are satisfied.

Build-only qualification without explicit signing, app launch or fixture execution:

```sh
companion/host/numeric-ui/script/build_and_run.sh --compile-only
companion/host/numeric-ui/script/test_offline.sh --compile-only
```

The helpers compile the current Swift models/services/views and the required
Objective-C bridge. Compile-only app output defaults to the dedicated project
`.tools/numeric-ui/source-only/StackChanPairing.app`; `STACKCHAN_UI_BUILD_ROOT`
can select another output directory. An existing `_CodeSignature` directory is
rejected in this mode. The compiler/linker may emit platform-default Mach-O code
signature data; no explicit codesign, installed-app replacement or activation is
performed by compile-only mode. It does not certify an artifact as never signed.

`STACKCHAN_OFFLINE_BUILD_ROOT` selects the fixture output/module-cache directory.
`--compile-only` on the offline helper compiles all six fixtures and runs none.
The default offline mode executes the fixtures without resource supervision.
For bounded process-fixture qualification, `script/run_process_fixtures.py` uses
the separate audited no-fork policy documented in `script/FOUNDATION-FIXTURES.md`.
Synthetic Python workers register and stop before their test bodies; the runner
checks kernel birth, lineage and session, records custody, then resumes them.
Real Foundation Process pipes, termination handlers and signal escalation remain
under test. The original strict single-group watchdog is retained byte-identical;
this runner explicitly permits owned child process groups within its session.
Compilation alone is never runtime acceptance.

`--build-only` retains the existing explicit ad-hoc signing and bundle verification
behavior. Other build-and-run modes can launch the app and are outside source-only
qualification. No helper should be run in a mode that exceeds the current operator's
authorization. The installed, idle app is not modified by these source checks.

Live operation requires separately approved bindings/configuration and existing
operator/device consent gates. `STACKCHAN_BINDING_CONFIG` selects an explicit
owner-only binding file; `HOME_LAB_ROOT` supplies the existing local host source.
Neither value is populated by the public source build. No real credential, private
key, current certificate validity or physical card content is inferred here.
The window has no token field, clipboard operation or credential log.

The worker anchors its finite deadline at robot Ready, including its startup and
control-loop margin. This does not prove boot continuity, physical audio timing,
route restoration or capture shutdown on actual hardware. See
[offline boundaries](COMPANION-OFFLINE.md) and [integration seams](HOME-INTEGRATION.md)
for the historical design context; current qualification evidence takes precedence
over earlier installed-image and certificate descriptions in those documents.

Current audio direction blocker: both existing public aliases expose both stream
directions. The app now checks actual channels/streams and does not create or
certify those duplex aliases. See [audio direction](../../AUDIO-DIRECTION.md).
