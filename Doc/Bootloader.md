# Multiplatform Bootloader Architecture (C++20)

This project implements a secure, modular, and portable two-level bootloader system for embedded devices using **C++20**. It is designed to support firmware updates over UART in sealed devices, starting with **STM32F4DISCOVERY**, and extendable to **ESP32** and **Nordic nRF52** platforms.

## Notes

- **Bootloader I** is always at `0x08000000` and never overwritten.
- **Bootloader II** and **Application** are mirrored in both halves.
- **Metadata** includes update flags, version, CRC, and signature.
- **Error log** can store boot failures, flash errors, or validation issues.
- **Certificate** space allows for secure signature verification in future updates.
- **Reserved space** gives flexibility for future features (e.g., rollback index, telemetry).

## Project Goals

- **Language**: C++20 (no dynamic allocation, no exceptions)
- **Style**: Pure register access; LL only if needed
- **Multiplatform**: STM32F4 → ESP32 → nRF52
- **No vendor lock-in**: Avoid STM32Cube/HAL
- **Free tools only**: GCC, OpenOCD, Python, mbedTLS
- **No external flash**: All firmware fits in internal flash
- **Secure updates**: CRC32/SHA256 + optional digital signature
- **Robust architecture**: Safe staging, rollback-ready, predictable vector tables

## Architecture Overview

### Bootloader Primary (Minimal, Static)

- Located at `0x08000000`
- Written in C++20 with direct register access
- Sets VTOR and jumps to Bootloader II
- Validates Bootloader II integrity
- Never updated remotely (only via SWD/JTAG)

### Bootloader Secendary (Configurable, Updatable)

- Located at `0x08004000`
- Written in portable C++20 with abstraction layer
- Validates staged firmware (CRC, signature)
- Applies update to application region
- Handles device-specific configuration

### Application

- Located at `0x08010000`
- Receives firmware via UART
- Stores firmware in staging area
- Sets update flag in metadata
- Continues running normally

## Firmware Update Flow

1. **Application Mode**
   - Receives firmware over UART
   - Writes to staging area (`0x08080000`)
   - Sets update flag in metadata (`0x080E0000`)

2. **On Reboot**
   - Bootloader I validates Bootloader II
   - Bootloader II checks metadata
   - If valid: flashes new firmware to app region
   - If invalid: skips update and boots existing app

## Security Features

- Integrity: CRC32 or SHA256
- Authenticity: RSA/ECDSA signature (optional)
- Validation: Performed by Bootloader II before flashing
- Public Key: Stored in Bootloader I

## Tools & Libraries

- Compiler: GCC (arm-none-eabi, xtensa)
- Flashing: OpenOCD, pyOCD, esptool
- Crypto: mbedTLS or tinycrypt
- Packaging: Python script (adds header, CRC, signature)

## Status

- Bootloader Primary: STM32F4 prototype complete
- Bootloader Secendary: Firmware validation logic in progress
- Application-side UART update handler
- Python firmware packaging tool
- ESP32 and Nordic ports

## Flash Update Flow

- Bootloader starts from Sector 0–4
- Loads new firmware image into RAM or external source
- Runs Flash erase/program routines from SRAM
- Erases old app sectors (e.g., Sectors 5–7)
- Programs new image
- Verifies CRC/signature
- Updates metadata/certificates
- Jumps to application (0x08020000)

## License

This project is released under the MIT License.
