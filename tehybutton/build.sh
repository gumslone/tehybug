#!/usr/bin/env bash
#
# Build the tehybutton firmware with arduino-cli.
#
#   ./build.sh [debug|release|all]   (default: release)
#
# Binaries land in build/<board>/<mode>/. The release esp8285 binary is
# also copied over the shipped tehybutton.ino.esp8285.bin.
#
set -euo pipefail

SKETCH_DIR="$(cd "$(dirname "$0")" && pwd)"
MODE="${1:-release}"

BOARDS=(
  "esp8266:esp8266:esp8285"
  "esp8266:esp8266:generic"
)

case "$MODE" in
  debug)   MODES=(debug) ;;
  release) MODES=(release) ;;
  all)     MODES=(release debug) ;;
  *)
    echo "usage: $0 [debug|release|all]" >&2
    exit 1
    ;;
esac

# Use the vendored libraries if they exist, otherwise the global ones.
LIB_FLAGS=()
if [ -d "$SKETCH_DIR/libraries" ]; then
  LIB_FLAGS=(--libraries "$SKETCH_DIR/libraries")
fi

for board in "${BOARDS[@]}"; do
  board_name="${board##*:}"
  for mode in "${MODES[@]}"; do
    out_dir="$SKETCH_DIR/build/$board_name/$mode"
    mkdir -p "$out_dir"

    extra_flags=()
    if [ "$mode" = "debug" ]; then
      extra_flags=(--build-property "compiler.cpp.extra_flags=-DDEBUG=1")
    fi

    echo "==> $board_name ($mode)"
    arduino-cli compile \
      -b "$board" \
      "${LIB_FLAGS[@]}" \
      "${extra_flags[@]}" \
      --output-dir "$out_dir" \
      "$SKETCH_DIR"

    ls -l "$out_dir/tehybutton.ino.bin"
  done
done

# Refresh the shipped binary from the esp8285 release build.
if [ -f "$SKETCH_DIR/build/esp8285/release/tehybutton.ino.bin" ]; then
  cp "$SKETCH_DIR/build/esp8285/release/tehybutton.ino.bin" \
     "$SKETCH_DIR/tehybutton.ino.esp8285.bin"
  echo "==> refreshed tehybutton.ino.esp8285.bin"
fi
