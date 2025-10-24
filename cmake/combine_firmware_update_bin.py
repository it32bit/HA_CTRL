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
# | BootSec Metadata     | 0x0801FC00    | 512 B  |
# | Application          | 0x08020000    | 383 KB |
# | App Metadata         | 0x0807FE00    | 512 B  |
#
# It pads each region with 0xFF to ensure proper alignment
# and fills gaps between regions.

import sys
import os

def pad_to_offset(f, target_offset):
    current_size = f.tell()
    if current_size > target_offset:
        raise ValueError(f"Data exceeds target offset {target_offset}")
    f.write(b'\xFF' * (target_offset - current_size))

def combine(app_bin, app_meta, sec_bin, sec_meta, output_path):
    # Constants
    APP_META_OFFSET = 458240       # 0x0807FE00
    SEC_META_OFFSET = 64512        # 0x0801FC00

    with open(app_bin, 'rb') as f_app, open(app_meta, 'rb') as f_app_meta, \
         open(sec_bin, 'rb') as f_sec, open(sec_meta, 'rb') as f_sec_meta, \
         open(output_path, 'wb') as out:

        # Write secondary bootloader
        sec_data = f_sec.read()
        out.write(sec_data)
        pad_to_offset(out, SEC_META_OFFSET)
        out.write(f_sec_meta.read())

        # Pad to start of application
        app_start_offset = 0x08020000 - 0x08000000  # Assuming app starts at 0x08020000
        pad_to_offset(out, app_start_offset)

        # Write application
        app_data = f_app.read()
        out.write(app_data)
        pad_to_offset(out, APP_META_OFFSET)
        out.write(f_app_meta.read())

    print(f"Combined firmware written to {output_path}")

if __name__ == '__main__':
    if len(sys.argv) != 6:
        print("Usage: combine_firmware.py <app.bin> <app_metadata.bin> <sec.bin> <sec_metadata.bin> <output.bin>")
        sys.exit(1)

    combine(*sys.argv[1:])
