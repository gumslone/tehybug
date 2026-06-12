#!/usr/bin/env bash
#
# Build the TeHyBug CO2 firmware with arduino-cli.
#
# Usage: ./build.sh [debug|release|all]   (default: release)
#
# Output goes to build/<board>/<mode>/. A release build also refreshes the
# prebuilt tehybug_co2_firmware.ino.<board>.bin files shipped in the repo.
#
# The firmware's DEBUG flag defaults to 0; debug builds inject -DDEBUG=1.

set -euo pipefail
cd "$(dirname "$0")"

SKETCH_NAME="tehybug_co2_firmware"
CORE="esp8266:esp8266"
BOARDS=(esp8285 generic)
MODE="${1:-release}"

# Board options matching the Arduino IDE settings the device is flashed with:
# 2MB flash (FS:64KB OTA:~992KB) and basic SSL ciphers. All other IDE
# settings (80 MHz CPU, 26 MHz crystal, nodemcu reset, lwIP v2 lower memory,
# flash VTables, legacy exceptions) are the core 2.7.4 defaults.
BOARD_OPTS="eesz=2M64,ssl=basic"

LIB_FLAGS=()
if [ -d libraries ]; then
  LIB_FLAGS=(--libraries libraries)
fi

build_one() {
  local board="$1" mode="$2"
  local out="build/${board}/${mode}"
  local extra_flags=()

  if [ "$mode" = "debug" ]; then
    extra_flags=(--build-property "compiler.cpp.extra_flags=-DDEBUG=1")
  fi

  echo "==> ${board} (${mode})"
  arduino-cli compile \
    --fqbn "${CORE}:${board}:${BOARD_OPTS}" \
    --output-dir "$out" \
    "${LIB_FLAGS[@]}" \
    "${extra_flags[@]}" \
    .

  if [ "$mode" = "release" ]; then
    cp "${out}/${SKETCH_NAME}.ino.bin" "${SKETCH_NAME}.ino.${board}.bin"
    echo "    refreshed ${SKETCH_NAME}.ino.${board}.bin"
  fi
}

case "$MODE" in
  debug | release)
    for board in "${BOARDS[@]}"; do build_one "$board" "$MODE"; done
    ;;
  all)
    for board in "${BOARDS[@]}"; do
      build_one "$board" release
      build_one "$board" debug
    done
    ;;
  *)
    echo "usage: $0 [debug|release|all]" >&2
    exit 1
    ;;
esac
