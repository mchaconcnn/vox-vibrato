#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
ARTEFACTS="$BUILD_DIR/VoxVibrato_artefacts/RelWithDebInfo"

"$ROOT_DIR/script/build_and_run.sh" --verify
mkdir -p "$HOME/Library/Audio/Plug-Ins/Components" "$HOME/Library/Audio/Plug-Ins/VST3"
ditto "$ARTEFACTS/AU/Vox Vibrato.component" \
      "$HOME/Library/Audio/Plug-Ins/Components/Vox Vibrato.component"
ditto "$ARTEFACTS/VST3/Vox Vibrato.vst3" \
      "$HOME/Library/Audio/Plug-Ins/VST3/Vox Vibrato.vst3"
xattr -cr "$HOME/Library/Audio/Plug-Ins/Components/Vox Vibrato.component"
xattr -cr "$HOME/Library/Audio/Plug-Ins/VST3/Vox Vibrato.vst3"
codesign --force --deep --sign - \
         "$HOME/Library/Audio/Plug-Ins/Components/Vox Vibrato.component"
codesign --force --deep --sign - \
         "$HOME/Library/Audio/Plug-Ins/VST3/Vox Vibrato.vst3"
killall -9 AudioComponentRegistrar >/dev/null 2>&1 || true
echo "Installed AU and VST3 for the current user."
