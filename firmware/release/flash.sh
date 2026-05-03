#!/bin/zsh

set -euo pipefail

[[ $# -ge 1 && -f "$1" ]] || exit 1

echo "flashing $1"
esptool --chip esp32s3 erase-flash
esptool --chip esp32s3 write-flash 0x0 $1