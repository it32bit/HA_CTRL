#!/usr/bin/env python3

# Usage:
# python3 gen_metadata.py firmware_version.txt app_metadata.bin

import sys
import os

def generate_metadata(version_txt_path, output_bin_path):
    if not os.path.exists(version_txt_path):
        print(f"ERROR: Version file not found: {version_txt_path}")
        sys.exit(1)

    with open(version_txt_path, 'r') as f:
        version_str = f.read().strip()

    if not version_str:
        print("ERROR: Version file is empty.")
        sys.exit(1)

    print(f"Read firmware version: '{version_str}'")

    # Convert to bytes (ASCII), pad to 512 bytes
    version_bytes = version_str.encode('ascii')
    padded = version_bytes.ljust(512, b'\x00')

    with open(output_bin_path, 'wb') as out:
        out.write(padded)

    print(f"Metadata written to {output_bin_path} ({len(padded)} bytes)")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: gen_metadata.py <version.txt> <output.bin>")
        sys.exit(1)

    version_txt_path = sys.argv[1]
    output_bin_path = sys.argv[2]
    generate_metadata(version_txt_path, output_bin_path)
