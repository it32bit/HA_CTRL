#!/usr/bin/env python3

# python3 combine_firmware_update_bin.py \
#     app.bin \
#     app_metadata.bin \
#     boot_sec.bin \
#     boot_sec_metadata.bin \
#     combined_output.bin

# | Component            | Flash Address | Size   |
# |----------------------|---------------|--------|
# | Secondary Bootloader | 0x08010000    | 64 KB  |
# | Application          | 0x08020000    | 383 KB |
# | App Metadata         | 0x0807FC00    | 512 B  |
# | BootSec Metadata     | 0x0807FE00    | 512 B  |
#
# It pads each region with 0xFF to ensure proper alignment
# and fills gaps between regions.


import sys
import os
import struct

# === Flash layout constants ===
FLASH_BASE_ADDR = 0x08000000

BOOTLOADER2_START = 0x08010000
BOOTLOADER2_SIZE  = 64 * 1024

APP_START         = 0x08020000
APP_SIZE          = (128 + 128 + 128) * 1024 - 1024  # 383KB
APP_METADATA_START = 0x0807FC00
APP_METADATA_SIZE  = 512
APP_CERT_START     = 0x0807FE00
APP_CERT_SIZE      = 512

# === Input arguments ===
if len(sys.argv) != 6:
    print(f"Usage: {sys.argv[0]} <app.bin> <app_metadata.bin> <boot_sec.bin> <boot_sec_metadata.bin> <out_combined.bin>")
    sys.exit(1)

app_bin_path         = sys.argv[1]
app_metadata_path    = sys.argv[2]
boot_sec_bin_path    = sys.argv[3]
boot_sec_metadata_path = sys.argv[4]
out_bin_path         = sys.argv[5]

# === Helper: Load binary and pad to expected size ===
def load_and_pad(path, expected_size):
    with open(path, "rb") as f:
        data = f.read()
    if len(data) > expected_size:
        raise ValueError(f"{path} exceeds expected size ({len(data)} > {expected_size})")
    return data + b'\xFF' * (expected_size - len(data))

# === Load binaries ===
boot_sec_bin     = load_and_pad(boot_sec_bin_path, BOOTLOADER2_SIZE)
boot_sec_meta    = load_and_pad(boot_sec_metadata_path, 512)
app_bin          = load_and_pad(app_bin_path, APP_SIZE)
app_metadata     = load_and_pad(app_metadata_path, 512)

# === Create memory map ===
combined_size = (APP_CERT_START + APP_CERT_SIZE) - BOOTLOADER2_START
combined_image = bytearray(b'\xFF' * combined_size)

def write_to_flash(offset_addr, data, label):
    offset = offset_addr - BOOTLOADER2_START
    print(f"{label}: Flash Addr=0x{offset_addr:08X}, Offset={offset}, Size={len(data)}")
    combined_image[offset:offset+len(data)] = data

# === Write components ===
write_to_flash(BOOTLOADER2_START, boot_sec_bin, "Secondary Bootloader")
write_to_flash(APP_START, app_bin, "Application")
write_to_flash(APP_METADATA_START, app_metadata, "App Metadata")
write_to_flash(APP_CERT_START, boot_sec_meta, "BootSec Metadata")

# === Save combined binary ===
with open(out_bin_path, "wb") as f:
    f.write(combined_image)

print(f"\nCombined binary created: {out_bin_path}")
