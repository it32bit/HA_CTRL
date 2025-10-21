# Multiplatform Bootloader Architecture (C++20)

This project implements a secure, modular, and portable two-level bootloader system for embedded devices using **C++20**. It is designed to support firmware updates over UART in sealed devices, starting with **STM32F4DISCOVERY**, and extendable to **ESP32** and **Nordic nRF52** platforms.

## Notes

- **Bootloader I** is always at `0x08000000` and never overwritten.
- **Bootloader II** and **Application** are mirrored in both halves.
- **Metadata** includes update flags, version, CRC, and signature.
- **Error log** can store boot failures, flash errors, or validation issues.
- **Certificate** space allows for secure signature verification in future updates.
- **Reserved space** gives flexibility for future features (e.g., rollback index, telemetry).

---

Would you like me to generate a matching `FirmwareMetadata` structure in C++20 that maps to this layout? Or a Python script to help package staged firmware with metadata?

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

### Bootloader I (Minimal, Static)

- Located at `0x08000000`
- Written in C++20 with direct register access
- Sets VTOR and jumps to Bootloader II
- Validates Bootloader II integrity
- Never updated remotely (only via SWD/JTAG)

### Bootloader II (Configurable, Updatable)

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

- Bootloader I: STM32F4 prototype complete
- Bootloader II: Firmware validation logic in progress
- Application-side UART update handler
- Python firmware packaging tool
- ESP32 and Nordic ports

---

## JIRA Epic: Bootloader Architecture (C++20, Multiplatform)

### Component: Bootloader I (Minimal, Static)

| Task Title                        | Description                                                                                      | Labels                                  |
|----------------------------------|--------------------------------------------------------------------------------------------------|------------------------------------------|
| BL1-001: Initialize MCU with pure register access | Implement clock setup, watchdog disable, and vector table configuration using direct register access (no HAL). | bootloader1, stm32, registers, startup  |
| BL1-002: Implement VTOR setup logic             | Write code to set the vector table offset to Bootloader II or Application depending on validation. | bootloader1, vtor, startup               |
| BL1-003: Validate Bootloader II integrity       | Add CRC32 or SHA256 check for Bootloader II before jumping.                                   | bootloader1, validation, security        |
| BL1-004: Implement jump to Bootloader II        | Use reinterpret_cast<void(*)()> to jump to Bootloader II after validation.                    | bootloader1, jump, startup               |

### Component: Bootloader II (Configurable, Updatable)

| Task Title                         | Description                                                                                       | Labels                                      |
|-----------------------------------|---------------------------------------------------------------------------------------------------|---------------------------------------------|
| BL2-001: Design FirmwareMetadata structure | Define metadata format with magic, version, CRC, signature, and flags.                           | bootloader2, metadata, security             |
| BL2-002: Implement firmware validation logic | Validate staged firmware using CRC32 and optional signature.                                     | bootloader2, validation, security           |
| BL2-003: Implement FlashDriver concept interface | Create a C++20 concept for platform-specific flash operations.                                   | bootloader2, concepts, abstraction          |
| BL2-004: Implement STM32 FlashDriver        | Write a concrete implementation of FlashDriver for STM32F4 using direct registers.               | bootloader2, stm32, flash, registers        |
| BL2-005: Implement FirmwareUpdater class    | Create a class that validates and flashes firmware from staging to application region.           | bootloader2, firmware, update               |
| BL2-006: Implement jump to application logic| Add logic to jump to application after successful update or fallback.                            | bootloader2, jump, startup                  |
| BL2-007: Add device-specific configuration hooks | Provide extension points for calibration, provisioning, or hardware setup.                      | bootloader2, config, customization          |

### Component: Application

| Task Title                          | Description                                                                     | Labels                          |
|------------------------------------|---------------------------------------------------------------------------------|---------------------------------|
| APP-001: Implement UART firmware receiver | Receive firmware over UART and write to staging area in flash.                  | application, uart, firmware     |
| APP-002: Write metadata and update flag   | After receiving firmware, write metadata and set update flag for Bootloader II. | application, metadata, update   |
| APP-003: Continue normal operation after update | Ensure application resumes normal function after firmware reception.           | application, runtime, update    |

### Component: Firmware Packaging Tool (Python)

| Task Title                          | Description                                                                       | Labels                          |
|------------------------------------|-----------------------------------------------------------------------------------|---------------------------------|
| TOOL-001: Create firmware packager script | Build a Python tool to wrap firmware binary with metadata and CRC.                | tooling, python, packaging      |
| TOOL-002: Add digital signature support   | Extend packager to sign firmware with RSA/ECDSA and embed signature.              | tooling, security, signature    |

### Component: Security

| Task Title                              | Description                                                                      | Labels                               |
|----------------------------------------|----------------------------------------------------------------------------------|--------------------------------------|
| SEC-001: Implement CRC32 and SHA256 utilities | Provide reusable hashing functions for firmware validation.                        | security, hashing, utilities         |
| SEC-002: Integrate mbedTLS for signature verification | Use mbedTLS or tinycrypt to verify firmware authenticity in Bootloader II.         | security, crypto, bootloader2        |
| SEC-003: Store public key in Bootloader I       | Embed public key in Bootloader I for signature verification.                       | security, bootloader1, key           |

### Component: Platform Porting

| Task Title                           | Description                                                               | Labels                          |
|-------------------------------------|---------------------------------------------------------------------------|---------------------------------|
| PORT-001: Port Bootloader I to ESP32 | Implement Bootloader I startup and jump logic for ESP32.                  | porting, esp32, bootloader1     |
| PORT-002: Port Bootloader II to ESP32| Implement FlashDriver and FirmwareUpdater for ESP32.                      | porting, esp32, bootloader2     |
| PORT-003: Port Bootloader I to nRF52 | Implement Bootloader I startup and jump logic for Nordic.                 | porting, nrf52, bootloader1     |
| PORT-004: Port Bootloader II to nRF52| Implement FlashDriver and FirmwareUpdater for Nordic.                     | porting, nrf52, bootloader2     |

### Jira

```Csv
Summary,Description,Issue Type,Labels,Component

BL1-001: Initialize MCU with pure register access,"Implement clock setup, watchdog disable, and vector table configuration using direct register access (no HAL).",Task,bootloader1,STM32F4
BL1-002: Implement VTOR setup logic,"Write code to set the vector table offset to Bootloader II or Application depending on validation.",Task,bootloader1,vtor
BL1-003: Validate Bootloader II integrity,"Add CRC32 or SHA256 check for Bootloader II before jumping.",Task,bootloader1,security
BL1-004: Implement jump to Bootloader II,"Use reinterpret_cast<void(*)()> to jump to Bootloader II after validation.",Task,bootloader1,jump

BL2-001: Design FirmwareMetadata structure,"Define metadata format with magic, version, CRC, signature, and flags.",Task,bootloader2,metadata
BL2-002: Implement firmware validation logic,"Validate staged firmware using CRC32 and optional signature.",Task,bootloader2,validation
BL2-003: Implement FlashDriver concept interface,"Create a C++20 concept for platform-specific flash operations.",Task,bootloader2,abstraction
BL2-004: Implement STM32 FlashDriver,"Write a concrete implementation of FlashDriver for STM32F4 using direct registers.",Task,bootloader2,stm32
BL2-005: Implement FirmwareUpdater class,"Create a class that validates and flashes firmware from staging to application region.",Task,bootloader2,firmware
BL2-006: Implement jump to application logic,"Add logic to jump to application after successful update or fallback.",Task,bootloader2,jump
BL2-007: Add device-specific configuration hooks,"Provide extension points for calibration, provisioning, or hardware setup.",Task,bootloader2,customization

APP-001: Implement UART firmware receiver,"Receive firmware over UART and write to staging area in flash.",Task,application,uart
APP-002: Write metadata and update flag,"After receiving firmware, write metadata and set update flag for Bootloader II.",Task,application,metadata
APP-003: Continue normal operation after update,"Ensure application resumes normal function after firmware reception.",Task,application,runtime

TOOL-001: Create firmware packager script,"Build a Python tool to wrap firmware binary with metadata and CRC.",Task,tooling,python
TOOL-002: Add digital signature support,"Extend packager to sign firmware with RSA/ECDSA and embed signature.",Task,tooling,security

SEC-001: Implement CRC32 and SHA256 utilities,"Provide reusable hashing functions for firmware validation.",Task,security,hashing
SEC-002: Integrate mbedTLS for signature verification,"Use mbedTLS or tinycrypt to verify firmware authenticity in Bootloader II.",Task,security,crypto
SEC-003: Store public key in Bootloader I,"Embed public key in Bootloader I for signature verification.",Task,security,key

PORT-001: Port Bootloader I to ESP32,"Implement Bootloader I startup and jump logic for ESP32.",Task,porting,esp32
PORT-002: Port Bootloader II to ESP32,"Implement FlashDriver and FirmwareUpdater for ESP32.",Task,porting,esp32
PORT-003: Port Bootloader I to nRF52,"Implement Bootloader I startup and jump logic for Nordic.",Task,porting,nrf52
PORT-004: Port Bootloader II to nRF52,"Implement FlashDriver and FirmwareUpdater for Nordic.",Task,porting,nrf52
```

## License

This project is released under the MIT License.
