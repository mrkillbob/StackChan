# Fork CI scope

`StackChan CI` runs for PRs to every base and for pushes to `main`,
`mira-desk-integration` and `claude/todo20-companion-lifecycle`. The candidate is stacked on PR3 correction `a1224dc`; publish it only after that
correction is on its intended base, or use a clearly stacked PR. PR1 later carries
the source to `main`; these companion paths are absent from current `main`.

The `All required checks pass` job requires firmware, Python and Swift lanes to
succeed, including on documentation changes. Failed, skipped or cancelled lanes
fail the aggregate. There are no path filters that strand a required check.
The check becomes a merge gate only after a separately approved GitHub ruleset
requires it. GitHub Actions' app integration ID is 15368.

The firmware lane prepares 16 exact vendor source files from immutable GitHub
revisions, applies only the existing reviewed application patch, and verifies
every resulting file against the generator SHA256 pins. No complete SDK, local
cache or hardware setup is required. Different existing files or symlinked
destinations are rejected. This subset is for host tests, not a firmware build.
CTest skips fail the job.

The seven additional host fixtures cover: advanced
configuration, firmware install policy, DMA completion fencing, camera authority,
camera wire lifecycle, screensaver selection policy and the actual generated
catalog renderer. Existing motion/navigation tests remain registered. Together
with PR3 correction this is 21 CTests. Assertions stay enabled in Release too.

The Python lane checks source syntax and runs the public synthetic trust and
fixture-custody model suites. The macOS lane compiles six Swift fixtures and runs
three pure models; it does not run the three process lifecycle fixtures.

These lanes do not build ESP-IDF, Android/iOS or the Go service. The Flutter app's
SDK constraint is Dart ^3.11.5. A proposed Flutter 3.47.7 tag exists, but the
previous proposal's Dart 3.13.5 pairing and hosted Android build have not been
qualified here. Add those jobs after reproducing dependency resolution, analysis
and debug signing/build on a clean SDK. Do not turn an unverified app lane into
a required gate or copy Hermes-specific jobs.

The correction's actual playback/preview consumer fixture exercises all 216
selectable effects at 0, 1,000 and 29,999 ms with a synthetic ESP-clock boundary.
This candidate additionally verifies playback time advances and Gears frames
change through both consumers. Existing bounded-clock/catalog tests remain.
These checks catch the clock rejection regression and a visible frozen consumer;
they do not measure real ESP32 frame latency or certify the reported device freeze.

Review policy is currently manual. A Codex summary that names the latest commit,
its no-unresolved-findings state and a bot-authored reaction establish different
parts of review provenance. A reaction by itself is not a SHA-bound approval.
Smart Detect's private integration setting cannot be established by repository
source or a PR reaction. Verify it in the Codex integration settings before
claiming parity. Do not manufacture bot approval with labels or reactions.

Upstream sync is a reviewed branch/PR operation (see CONTRIBUTING.md), not an
unattended scheduled merge. This workflow has no secrets, publishing, self-hosted
runner or write permissions. Protection, Actions policy and review integration
settings are separate administrative decisions.
