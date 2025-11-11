#!/bin/bash
ELF="$1"

# Extract raw bytes from the firmware version section
VERSION_HEX=$(arm-none-eabi-objdump -s --section=.firmware_version "$ELF" | tail -n +5 | head -n 1 | awk '{print $2}')

if [ -z "$VERSION_HEX" ]; then
  echo ">>> WARNING: .firmware_version section not found in $ELF" >&2
  exit 1
fi

MAJOR=$((0x${VERSION_HEX:0:2}))
MINOR=$((0x${VERSION_HEX:2:2}))

echo "${MAJOR}.${MINOR}"
