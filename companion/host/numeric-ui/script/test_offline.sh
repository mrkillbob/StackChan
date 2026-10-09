#!/bin/bash
set -euo pipefail
UI_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_ROOT="/Users/mikedemott/Developer/AgentWork/StackChan"
BUILD_ROOT="$PROJECT_ROOT/.tools/numeric-ui/offline-tests"
mkdir -p "$BUILD_ROOT"
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$PROJECT_ROOT/.tools/numeric-ui/module-cache" "$UI_ROOT/Models/PairingStatus.swift" "$UI_ROOT/Tests/StatusChecks.swift" -o "$BUILD_ROOT/status-checks"
"$BUILD_ROOT/status-checks"
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$PROJECT_ROOT/.tools/numeric-ui/module-cache" "$UI_ROOT/Models/PairingStatus.swift" "$UI_ROOT/Services/PairingController.swift" "$UI_ROOT/Tests/LifecycleChecks.swift" -o "$BUILD_ROOT/lifecycle-checks"
"$BUILD_ROOT/lifecycle-checks"
