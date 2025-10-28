#!/usr/bin/env python3

# python3 gen_metadata.py firmware_version.txt ha-ctrl-app.bin app_metadata.bin
#
# This will produce:
#   (+) app_metadata.bin: binary metadata matching C++ struct from firmware_metadata.hpp
#   (+) app_metadata.json: human-readable metadata for tooling

import sys
import os
import json
import struct
import hashlib
# import zlib
import time
import crcmod

def parse_version(version_str):
    parts = version_str.strip().split('.')
    if len(parts) == 2:
        parts.append('0')  # Default patch to 0
    if len(parts) != 3:
        raise ValueError("Version must be in format Major.Minor.Patch (e.g. 1.2.3)")
    major, minor, patch = map(int, parts)
    return (major << 16) | (minor << 8) | patch

def generate_metadata(version_txt_path, firmware_bin_path, output_bin_path):
    # Read version string
    with open(version_txt_path, 'r', encoding='utf-8', errors='replace') as f:
        version_str = f.read().strip()

    version_encoded = parse_version(version_str)

    # Read firmware binary
    with open(firmware_bin_path, 'rb') as f:
        firmware_data = f.read()

    # firmware_crc32 = zlib.crc32(firmware_data) & 0xFFFFFFFF  # Ensure unsigned 32-bit

    # It will match with STM32F4
    app_crc32 = crcmod.mkCrcFun(0x104C11DB7, rev=False, initCrc=0xFFFFFFFF, xorOut=0x00000000)
    firmware_crc32 = app_crc32(firmware_data)

    firmware_size = len(firmware_data)
    firmware_hash = hashlib.sha256(firmware_data).digest()

    # Metadata fields
    magic = 0xDEADBEEF
    timestamp = int(time.time())
    reserved = (0, 0)
    # STM32F407 = F 4 0 7
    mcu_name = 0x46343037

    mcu_name_ascii = mcu_name.to_bytes(4, byteorder='big').decode('ascii')

    # Pack binary metadata
    metadata_struct = struct.pack(
        '<IIIIIIII32s',  # Corrected format string
        magic,
        version_encoded,
        timestamp,
        firmware_size,
        firmware_crc32,
        reserved[1],
        reserved[0],
        mcu_name,
        firmware_hash,
    )

    with open(output_bin_path, 'wb') as out:
        out.write(metadata_struct)

    print(f"Timestamp being written: {timestamp} → {time.ctime(timestamp)}")
    print(f"Structured metadata written to {output_bin_path} ({len(metadata_struct)} bytes)")

    # Write JSON companion
    output_json_path = os.path.splitext(output_bin_path)[0] + '.json'
    metadata_json = {
        "memory": {
            "magic": hex(magic),
            "version_encoded": hex(version_encoded),
            "build_timestamp": hex(timestamp),
            "firmware_size": hex(firmware_size),
            "firmware_crc32": hex(firmware_crc32),
            "reserved": list(reserved),
            "mcu_name": hex(mcu_name),
            "firmware_sha256": firmware_hash.hex()
        },
        "info": {
            "version": version_str,
            "build_timestamp": time.strftime("%Y-%m-%d %H:%M:%S %Z", time.localtime(timestamp)),
            "firmware_size": firmware_size,
            "mcu_name": mcu_name_ascii
        }
    }

    with open(output_json_path, 'w') as json_out:
        json.dump(metadata_json, json_out, indent=4)

    print(f"JSON metadata written to {output_json_path}")

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: gen_metadata.py <version.txt> <firmware.bin> <output_metadata.bin>")
        sys.exit(1)

    version_txt_path = sys.argv[1]
    firmware_bin_path = sys.argv[2]
    output_bin_path = sys.argv[3]

    generate_metadata(version_txt_path, firmware_bin_path, output_bin_path)
