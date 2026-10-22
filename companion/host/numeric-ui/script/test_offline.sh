#!/bin/bash
set -euo pipefail
MODE="${1:-run}"
case "$MODE" in run|--compile-only) ;; *) echo "usage: $0 [--compile-only]" >&2; exit 2 ;; esac
UI_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_ROOT="${STACKCHAN_ROOT:-$(cd "$UI_ROOT/../../.." && pwd)}"
BUILD_ROOT="${STACKCHAN_OFFLINE_BUILD_ROOT:-$PROJECT_ROOT/.tools/numeric-ui/offline-tests}"
mkdir -p "$BUILD_ROOT"
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Tests/StatusChecks.swift" -o "$BUILD_ROOT/status-checks"
if [[ "$MODE" != "--compile-only" ]]; then "$BUILD_ROOT/status-checks"; fi
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Services/AudioRouteCatalog.swift" "$UI_ROOT/Services/PairingController.swift" "$UI_ROOT/Tests/LifecycleChecks.swift" -o "$BUILD_ROOT/lifecycle-checks"
if [[ "$MODE" != "--compile-only" ]]; then "$BUILD_ROOT/lifecycle-checks"; fi

/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Services/AudioRouteCatalog.swift" "$UI_ROOT/Services/PairingController.swift" "$UI_ROOT/Tests/CompanionChecks.swift" -o "$BUILD_ROOT/companion-checks"
if [[ "$MODE" != "--compile-only" ]]; then "$BUILD_ROOT/companion-checks"; fi
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Tests/RememberedDeviceChecks.swift" -o "$BUILD_ROOT/remembered-checks"
if [[ "$MODE" != "--compile-only" ]]; then "$BUILD_ROOT/remembered-checks"; fi

/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Services/AudioRouteCatalog.swift" "$UI_ROOT/Services/PairingController.swift" "$UI_ROOT/Tests/PersistentChecks.swift" -o "$BUILD_ROOT/persistent-checks"
if [[ "$MODE" != "--compile-only" ]]; then "$BUILD_ROOT/persistent-checks"; fi
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Tests/MiraLifecycleChecks.swift" -o "$BUILD_ROOT/mira-lifecycle-checks"
if [[ "$MODE" != "--compile-only" ]]; then "$BUILD_ROOT/mira-lifecycle-checks"; fi
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Services/PersistentRegistryReader.swift" "$UI_ROOT/Tests/W1LifecycleChecks.swift" -o "$BUILD_ROOT/w1-lifecycle-checks"
if [[ "$MODE" != "--compile-only" ]]; then "$BUILD_ROOT/w1-lifecycle-checks"; fi
