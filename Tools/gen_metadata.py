#!/usr/bin/env python3

# python3 gen_metadata.py ha-ctrl-app.elf app_metadata.bin
# python3 gen_metadata.py ha-ctrl-sec.elf bootsec_metadata.bin

import sys
from elftools.elf.elffile import ELFFile

def extract_firmware_version(elf_path, output_path):
    with open(elf_path, 'rb') as f:
        elf = ELFFile(f)

        section = elf.get_section_by_name('.firmware_version')
        if section is None:
            print(f"ERROR: Section '.firmware_version' not found in {elf_path}")
            sys.exit(1)

        data = section.data()
        print(f"Extracted {len(data)} bytes from '.firmware_version' in {elf_path}")

        with open(output_path, 'wb') as out:
            out.write(data)
        print(f"Metadata written to {output_path}")

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: gen_metadata.py <input.elf> <output.bin>")
        sys.exit(1)

    elf_path = sys.argv[1]
    output_path = sys.argv[2]
    extract_firmware_version(elf_path, output_path)
