#!/bin/bash
set -euo pipefail
UI_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_ROOT="/Users/mikedemott/Developer/AgentWork/StackChan"
BUILD_ROOT="${STACKCHAN_OFFLINE_BUILD_ROOT:-$PROJECT_ROOT/.tools/numeric-ui/offline-tests}"
mkdir -p "$BUILD_ROOT"
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Tests/StatusChecks.swift" -o "$BUILD_ROOT/status-checks"
"$BUILD_ROOT/status-checks"
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Services/AudioRouteCatalog.swift" "$UI_ROOT/Services/PairingController.swift" "$UI_ROOT/Tests/LifecycleChecks.swift" -o "$BUILD_ROOT/lifecycle-checks"
"$BUILD_ROOT/lifecycle-checks"

/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Services/AudioRouteCatalog.swift" "$UI_ROOT/Services/PairingController.swift" "$UI_ROOT/Tests/CompanionChecks.swift" -o "$BUILD_ROOT/companion-checks"
"$BUILD_ROOT/companion-checks"
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Tests/RememberedDeviceChecks.swift" -o "$BUILD_ROOT/remembered-checks"
"$BUILD_ROOT/remembered-checks"

/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$BUILD_ROOT/module-cache" "$UI_ROOT"/Models/*.swift "$UI_ROOT/Services/AudioRouteCatalog.swift" "$UI_ROOT/Services/PairingController.swift" "$UI_ROOT/Tests/PersistentChecks.swift" -o "$BUILD_ROOT/persistent-checks"
"$BUILD_ROOT/persistent-checks"
