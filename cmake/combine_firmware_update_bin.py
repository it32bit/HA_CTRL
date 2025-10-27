#!/usr/bin/env python3

# python3 combine_firmware_update_bin.py \
#     app.bin \
#     app_metadata.bin \
#     boot_sec.bin \
#     boot_sec_metadata.bin \
#     Platform\Platform_MCU\Inc\flash_layout.hpp \
#     combined_output.bin

# | Component            | Flash Address | Size   |
# |----------------------|---------------|--------|
# | Secondary Bootloader | 0x08090000    | 64 KB  |
# | BootSec Metadata     | 0x0809FC00    | 512 B  |
# | BootSec Cert         | 0x0809FE00    | 512 B  |
# | Application          | 0x080A0000    | 383 KB |
# | App Metadata         | 0x080FFC00    | 512 B  |
# | App Cert             | 0x080FFE00    | 512 B  |
#
# It pads each region with 0xFF to ensure proper alignment
# and fills gaps between regions.

import sys
import os
import re

def extract_address(header_path, symbol_name):
    with open(header_path, 'r') as f:
        content = f.read()
    match = re.search(rf'{symbol_name}\s*=\s*0x([0-9A-Fa-f]+)', content)
    if not match:
        raise ValueError(f"Symbol {symbol_name} not found in {header_path}")
    return int(match.group(1), 16) - 0x08000000  # Convert to offset

def pad_to_offset(f, target_offset):
    current_size = f.tell()
    if current_size > target_offset:
        raise ValueError(f"Data exceeds target offset {target_offset}")
    f.write(b'\xFF' * (target_offset - current_size))

def combine(app_bin, app_meta, sec_bin, sec_meta, header_path, output_path):

    FLASH_BASE_ADDR = extract_address(header_path, 'FLASH_BASE_ADDR')
    BOOTLOADER2_OFFSET = extract_address(header_path, 'NEW_BOOTLOADER2_START') - FLASH_BASE_ADDR
    OFFSET = BOOTLOADER2_OFFSET

    SEC_META_OFFSET = extract_address(header_path, 'NEW_BOOTLOADER2_METADATA_START') - OFFSET
    APP_OFFSET = extract_address(header_path, 'NEW_APP_START') - OFFSET
    APP_META_OFFSET = extract_address(header_path, 'NEW_APP_METADATA_START') - OFFSET

    with open(app_bin, 'rb') as f_app, open(app_meta, 'rb') as f_app_meta, \
         open(sec_bin, 'rb') as f_sec, open(sec_meta, 'rb') as f_sec_meta, \
         open(output_path, 'wb') as out:

        # Write secondary bootloader at start
        out.write(f_sec.read())

        # Write bootloader metadata
        pad_to_offset(out, SEC_META_OFFSET)
        out.write(f_sec_meta.read())

        # Write application
        pad_to_offset(out, APP_OFFSET)
        out.write(f_app.read())

        # Write application metadata
        pad_to_offset(out, APP_META_OFFSET)
        out.write(f_app_meta.read())

        final_size = out.tell()
        print(f"Combined firmware written to {output_path} ({final_size / 1024:.2f} KB)")

if __name__ == '__main__':
    if len(sys.argv) != 7:
        print("Usage: combine_firmware.py <app.bin> <app_metadata.bin> <sec.bin> <sec_metadata.bin> <output.bin> <flash_layout.hpp>")
        sys.exit(1)

    combine(*sys.argv[1:])
