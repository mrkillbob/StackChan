#!/bin/bash
set -euo pipefail
MODE="${1:-run}"
TASK_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_ROOT="/Users/mikedemott/Developer/AgentWork/StackChan"
BUNDLE="$PROJECT_ROOT/.tools/numeric-ui/StackChanPairing.app"
mkdir -p "$BUNDLE/Contents/MacOS" "$PROJECT_ROOT/.tools/numeric-ui/module-cache"
/usr/bin/nice -n 20 /usr/bin/xcrun swiftc -swift-version 5 -module-cache-path "$PROJECT_ROOT/.tools/numeric-ui/module-cache" -parse-as-library "$TASK_ROOT"/Models/*.swift "$TASK_ROOT"/Services/*.swift "$TASK_ROOT"/Views/*.swift "$TASK_ROOT"/App/*.swift -o "$BUNDLE/Contents/MacOS/StackChanPairing"
cat > "$BUNDLE/Contents/Info.plist" <<'PLIST'
<?xml version="1.0" encoding="UTF-8"?><plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>StackChanPairing</string>
<key>CFBundleIdentifier</key><string>local.mrkillbob.StackChanPairing</string>
<key>CFBundleName</key><string>Pair StackChan</string>
<key>CFBundlePackageType</key><string>APPL</string>
<key>LSMinimumSystemVersion</key><string>14.0</string>
<key>NSPrincipalClass</key><string>NSApplication</string>
</dict></plist>
PLIST
case "$MODE" in
  --build-only|--verify) test -x "$BUNDLE/Contents/MacOS/StackChanPairing" ;;
  run) /usr/bin/open -n "$BUNDLE" ;;
  --debug) /usr/bin/xcrun lldb -- "$BUNDLE/Contents/MacOS/StackChanPairing" ;;
  --logs|--telemetry) /usr/bin/open -n "$BUNDLE" ;;
  *) echo "usage: $0 [run|--build-only|--verify|--debug|--logs|--telemetry]" >&2; exit 2 ;;
esac
