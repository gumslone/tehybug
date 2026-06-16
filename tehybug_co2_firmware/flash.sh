#!/usr/bin/env bash
set -euo pipefail

# BugZapper CLI flasher for the TeHyBug CO2 sensor. Thin wrapper around the
# bundled tools/bugzapper/flash.sh that defaults to the shipped ESP8285
# release binary when no -f FILE is given. All flash.sh options are passed
# through (run ./flash.sh -h for the full list).
#
# Examples:
#   ./flash.sh                 # flash tehybug_co2_firmware.ino.esp8285.bin
#   ./flash.sh -e              # erase all flash, then write it
#   ./flash.sh -l              # list serial ports
#   ./flash.sh -f build/esp8285/debug/tehybug_co2_firmware.ino.bin

DIR="$(cd "$(dirname "$0")" && pwd)"

# Default to the shipped ESP8285 release binary unless the user picks one.
has_f=0
for arg in "$@"; do
  if [ "$arg" = "-f" ]; then has_f=1; break; fi
done
if [ "$has_f" -eq 0 ]; then
  set -- -f "$DIR/tehybug_co2_firmware.ino.esp8285.bin" "$@"
fi

exec "$DIR/tools/bugzapper/flash.sh" "$@"
