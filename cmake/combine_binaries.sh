#!/bin/bash
set -e

# === Input Arguments ===
APP_ELF="$1"
BOOT_SEC_ELF="$2"
BOOT_PRIM_ELF="$3"
APP_BIN="$4"
BOOT_SEC_BIN="$5"
BOOT_PRIM_BIN="$6"
OUT_BIN="$7"

# === Helper: Extract start address of .text section ===
get_text_addr() {
    arm-none-eabi-objdump -h "$1" | awk '/\.text/ {print $4}' | head -n 1
}

# === Helper: Get size of text + data + bss ===
get_elf_size() {
    arm-none-eabi-size "$1" | tail -n 1 | awk '{print $1 + $2 + $3}'
}

# === Extract start addresses from ELF files ===
APP_START_HEX=$(get_text_addr "$APP_ELF")
BOOT_SEC_START_HEX=$(get_text_addr "$BOOT_SEC_ELF")
BOOT_PRIM_START_HEX=$(get_text_addr "$BOOT_PRIM_ELF")

APP_START=$((0x$APP_START_HEX))
BOOT_SEC_START=$((0x$BOOT_SEC_START_HEX))
BOOT_PRIM_START=$((0x$BOOT_PRIM_START_HEX))

# === Extract sizes ===
APP_SIZE=$(get_elf_size "$APP_ELF")
BOOT_SEC_SIZE=$(get_elf_size "$BOOT_SEC_ELF")
BOOT_PRIM_SIZE=$(get_elf_size "$BOOT_PRIM_ELF")

APP_END=$((APP_START + APP_SIZE))
BOOT_SEC_END=$((BOOT_SEC_START + BOOT_SEC_SIZE))
BOOT_PRIM_END=$((BOOT_PRIM_START + BOOT_PRIM_SIZE))

# === Print extracted info ===
echo "App        : Start=0x$(printf '%08X' $APP_START), Size=${APP_SIZE} bytes, End=0x$(printf '%08X' $APP_END)"
echo "BootSec    : Start=0x$(printf '%08X' $BOOT_SEC_START), Size=${BOOT_SEC_SIZE} bytes, End=0x$(printf '%08X' $BOOT_SEC_END)"
echo "BootPrim   : Start=0x$(printf '%08X' $BOOT_PRIM_START), Size=${BOOT_PRIM_SIZE} bytes, End=0x$(printf '%08X' $BOOT_PRIM_END)"

# === Validate that addresses are in correct order ===
if (( BOOT_PRIM_END > BOOT_SEC_START )); then
    echo "ERROR: BootPrim overlaps with BootSec"
    exit 1
fi

if (( BOOT_SEC_END > APP_START )); then
    echo "ERROR: BootSec overlaps with App"
    exit 1
fi

# === Calculate padding ===
PAD1_SIZE=$((BOOT_SEC_START - BOOT_PRIM_END))
PAD2_SIZE=$((APP_START - BOOT_SEC_END))

echo "Padding between BootPrim → BootSec: ${PAD1_SIZE} bytes"
echo "Padding between BootSec → App    : ${PAD2_SIZE} bytes"

# === Create temporary padding files ===
PAD1_BIN=$(mktemp)
PAD2_BIN=$(mktemp)

dd if=/dev/zero bs=1 count=$PAD1_SIZE of="$PAD1_BIN" status=none
dd if=/dev/zero bs=1 count=$PAD2_SIZE of="$PAD2_BIN" status=none

# === Combine binaries ===
cat "$BOOT_PRIM_BIN" "$PAD1_BIN" "$BOOT_SEC_BIN" "$PAD2_BIN" "$APP_BIN" > "$OUT_BIN"
echo "Combined binary created: $OUT_BIN"

# === Cleanup ===
rm -f "$PAD1_BIN" "$PAD2_BIN"
