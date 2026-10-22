#!/bin/bash
# Offline Python suites for the Companion host (streaming/). Excludes test_remote_gateway.py,
# whose two fixtures bind a localhost WebSocket listener; run it separately only with approval.
set -euo pipefail
UI_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
HOST_ROOT="$(cd "$UI_ROOT/.." && pwd)"
PROJECT_ROOT="/Users/mikedemott/Developer/AgentWork/StackChan"
PYTHON="${STACKCHAN_PYTHON:-/opt/homebrew/bin/python3.14}"
export PYTHONDONTWRITEBYTECODE=1
export PYTHONPATH="/Users/mikedemott/Homelab/src:$PROJECT_ROOT/bridge/host-adapter:$PROJECT_ROOT/bridge/local-endpoint:$PROJECT_ROOT/.tools/bridge-python:$HOST_ROOT/native-audio:$HOST_ROOT/streaming:$HOST_ROOT"
cd "$HOST_ROOT/streaming"
modules=()
# STACKCHAN_SKIP_MODULES: space-separated module names to skip when their third-party
# dependency (pyserial, the HomeLab desk_robot package) is not installed. Skips are printed.
skip=" test_remote_gateway ${STACKCHAN_SKIP_MODULES:-} "
for f in test_*.py; do
  if [[ "$skip" == *" ${f%.py} "* ]]; then echo "SKIPPED ${f%.py}" >&2; continue; fi
  modules+=("${f%.py}")
done
exec "$PYTHON" -B -m unittest "${modules[@]}"
