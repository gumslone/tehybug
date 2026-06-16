#!/usr/bin/env bash
set -euo pipefail

# BugZapper GUI for the TeHyBug CO2 sensor: pick the serial port, firmware,
# baud and flash mode, flash the ESP8285, and watch the serial output in one
# window. esptool is bundled in the submodule (pure python), so flashing needs
# no install — only a python3 with tkinter.
#
# Usage: ./bugzapper.sh [firmware-dir]
#   firmware-dir   folder to list .bin files from (default: this project dir,
#                  which holds the shipped tehybug_co2_firmware.ino.*.bin)

DIR="$(cd "$(dirname "$0")" && pwd)"

export BUGZAPPER_TITLE="TeHyBug CO2 — BugZapper"
export BUGZAPPER_ICON="$DIR/images/bugzapper_icon.png"
# Default the firmware list to this project dir (where the .bin files live),
# unless the user passes their own folder.
export BUGZAPPER_FW_DIR="${1:-$DIR}"

exec "$DIR/tools/bugzapper/bugzapper.sh" "$@"
