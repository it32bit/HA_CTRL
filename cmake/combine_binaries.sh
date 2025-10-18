#!/bin/bash
APP_ELF="$1"
BOOT_ELF="$2"
BOOT_BIN="$3"
APP_BIN="$4"
PAD_BIN="$5"
COMBINED_BIN="$6"

# Extract app start address from .text section
APP_START=$(arm-none-eabi-objdump -h "$APP_ELF" | grep '.text' | awk '{print $4}')
BOOT_SIZE=$(arm-none-eabi-size "$BOOT_ELF" | tail -n 1 | awk '{print $1 + $2 + $3}')
APP_START_DEC=$((0x$APP_START))
BOOT_END=$((0x08000000 + $BOOT_SIZE))

# Validate alignment
if [ "$BOOT_END" -gt "$APP_START_DEC" ]; then
    echo ">>> ERROR: Bootloader overlaps with application!"
    echo "Bootloader ends at: 0x$(printf '%08X' $BOOT_END)"
    echo "App starts at     : 0x$APP_START"
    exit 1
fi

# Calculate padding
PAD_SIZE=$((APP_START_DEC - BOOT_END))

if [ "$PAD_SIZE" -eq 0 ]; then
    echo "No padding needed — bootloader ends exactly at app start."
else
    echo "Padding required: $PAD_SIZE bytes"
    dd if=/dev/zero bs=1 count=$PAD_SIZE of="$PAD_BIN"
fi

# Combine binaries
cat "$BOOT_BIN" "$PAD_BIN" "$APP_BIN" > "$COMBINED_BIN"
echo "Combined binary created at $COMBINED_BIN"
