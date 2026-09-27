#!/usr/bin/env bash
set -euo pipefail

MODE="${1:-run}"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
APP_NAME="Vox Vibrato"
APP_BUNDLE="$BUILD_DIR/VoxVibrato_artefacts/RelWithDebInfo/Standalone/$APP_NAME.app"

pkill -x "$APP_NAME" >/dev/null 2>&1 || true

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=RelWithDebInfo

case "$MODE" in
  run)
    cmake --build "$BUILD_DIR" --target VoxVibrato_Standalone --parallel
    /usr/bin/open -n "$APP_BUNDLE"
    ;;
  --debug|debug)
    cmake --build "$BUILD_DIR" --target VoxVibrato_Standalone --parallel
    lldb -- "$APP_BUNDLE/Contents/MacOS/$APP_NAME"
    ;;
  --logs|logs)
    cmake --build "$BUILD_DIR" --target VoxVibrato_Standalone --parallel
    /usr/bin/open -n "$APP_BUNDLE"
    /usr/bin/log stream --info --style compact --predicate "process == \"$APP_NAME\""
    ;;
  --telemetry|telemetry)
    cmake --build "$BUILD_DIR" --target VoxVibrato_Standalone --parallel
    /usr/bin/open -n "$APP_BUNDLE"
    /usr/bin/log stream --info --style compact --predicate 'subsystem == "com.marcochaconmora.voxvibrato"'
    ;;
  --verify|verify)
    cmake --build "$BUILD_DIR" --target VoxVibrato_AU VoxVibrato_VST3 VoxVibratoTests --parallel
    ctest --test-dir "$BUILD_DIR" --output-on-failure
    test -d "$BUILD_DIR/VoxVibrato_artefacts/RelWithDebInfo/AU/$APP_NAME.component"
    test -d "$BUILD_DIR/VoxVibrato_artefacts/RelWithDebInfo/VST3/$APP_NAME.vst3"
    echo "AU, VST3, and DSP tests verified."
    ;;
  *)
    echo "usage: $0 [run|--debug|--logs|--telemetry|--verify]" >&2
    exit 2
    ;;
esac
