# Bootloader Architecture

Project goals:

- Language: C++20
- Style: Pure register access, no dynamic allocation, no exceptions
- Design: One .cpp file with constexpr configuration, enum class for states, and inline functions
- Multiplatform-ready: STM32F4 → ESP32 → nRF52
- No vendor lock-in: Avoid STM32Cube/HAL; use direct register access or LL only if needed
- Free and open tools/libraries only
- No external flash: All firmware and bootloaders must fit in internal flash
- Secure and reliable firmware update mechanism
- Use constexpr for flash layout, enum class for boot states, and [[nodiscard]] for critical checks.

## General Concept: Two-Level Bootloader

Advantage:

- Minimal attack surface, stable recovery point even if Bootloader II is corrupted.

### Bootloader I (Minimal, Static, C++20, Platform-Specific, Always at 0x08000000)

Purpose:

- Language: C++20 (but minimal use of advanced features)
- Never updated remotely — only via programmer
- Always at 0x08000000

Responsibilities:

- Initializes MCU and sets VTOR to application vector table.”
- Validates and jumps to Bootloader II
- Verify Bootloader II integrity (CRC/SHA)
- Integrity check of Bootloader II (e.g., CRC, hash)
- Jump to Bootloader II using reinterpret_cast<void(*)()>

### Bootloader II (Portable, C++20, Configurable, Updatable)

Purpose:

- Language: Full C++20 with modules, ranges, concepts, and strong typing
- At fixed address (e.g., 0x08004000)
- Bootloader II can be updated via application or host command, with Bootloader 1 verifying integrity before activation.”

Responsibilities:

- Firmware validation (Integrity: CRC, signature)
- Applies update if valid
- Handles device-specific config (e.g., calibration, provisioning)
- Flashes new firmware from staging area to application region.

Design:

- Platform abstraction via concepts and template interface
- Firmware validation via std::span, std::array, and constexpr hashing
- Flash programming via platform-specific FlashDriver class
- Update logic in FirmwareUpdater class with state machine

Use std::ranges, std::bit_cast, consteval for compile-time checks, and std::variant for error handling.

### Application (Portable, C++20)

Purpose:

- At fixed address (e.g., 0x08010000)
- Receives firmware via UART
- Writes to staging area in flash
- Continues running normally
- Sets update flag in metadata

Responsibilities:

- Normal operation
- Trigger update via flag or command

## Firmware Switching Strategies

Goals:

- Using onboard flash only
- Bootloader I fixed
- Bootloader II for customization
- Safe updates via UART

### Option A: Flash Overwrite (Fixed App + Boot2)

- New firmware is written to a staging area
- On reboot, Bootloader I validates and copies firmware to the fixed application address (e.g., 0x08010000)
- Bootloader II remains fixed and is reused

#### (+) Pros A

- Simpler memory layout
- Easier to manage vector table (VTOR always points to fixed app address)
- Less fragmentation in flash
- Bootloader II can be reused across updates

#### (-) Cons A

- Flashing takes time and power — risk of power loss during overwrite
- Requires full erase/write cycle on every update
- No rollback unless explicitly implemented

### Option B: Firmware Slot Switching (Multiple App Regions)

- Maintain two application slots (e.g., App A and App B)
- Bootloader II selects which one to boot based on metadata
- New firmware is written to the inactive slot
- On success, metadata is updated to mark it active

#### (+) Pros B

- Safe updates: old firmware remains intact until new one is verified
- Easy rollback if new firmware fails
- No need to erase running app

#### (-) Cons B

- Requires double the flash space for applications
- More complex boot logic (slot selection, versioning)
- VTOR must be dynamically set to selected app’s vector table

### Option C: Hybrid Strategy

| Component       | Address       | Notes           |
|----------------|---------------|------------------|
| Bootloader I    | 0x08000000    |                  |
| Bootloader II   | 0x08004000    | Fixed address    |
| Application     | 0x08010000    | Fixed            |
| Staging Area    | 0x08080000    |                  |

On update:

- App writes new firmware to staging
- Bootloader II validates and copies to App region
- Optional: keep last known-good firmware in staging for rollback

#### (+) Pros C

- Simplicity of fixed layout
- Safety via staging + validation
- Flexibility via Bootloader II customization
- Portability across STM32, ESP32, Nordic

## Comparison Option A and Option C Hybrid

| Feature                    | Option A (Flash Overwrite)                    | Your Hybrid Approach                                 |
|----------------------------|-----------------------------------------------|------------------------------------------------------|
| Memory layout              | Fixed: App always at same address             | Fixed: App, Boot2, staging all defined               |
| Firmware update timing     | Bootloader copies new firmware                | App receives firmware, Bootloader applies            |
| Update safety              | Risky: overwrite during boot                  | Safer: firmware stored first, verified later         |
| Rollback support           | Manual or none                                | Possible: staging area can hold fallback             |
| Bootloader II presence     | Optional or absent                            | Required: handles config, validation                 |
| Flash wear                 | Frequent erase of app region                  | App region untouched until verified                  |
| Vector table management    | Simple: fixed VTOR                            | Simple: Boot1 sets VTOR to fixed app                 |
| Customization flexibility  | Limited                                       | High: Boot2 can configure hardware, modes            |

Hybrid model is designed for:

- Robustness: Firmware is never flashed blindly — it’s verified first.
- Modularity: Bootloader II can evolve independently (e.g., add encryption, config UI).
- Portability: Works across STM32, ESP32, Nordic — no need for dual-slot complexity.
- Simplicity: Fixed addresses mean predictable vector tables and easier debugging.

## Firmware Validation

Integrity:

- CRC32 or SHA256
- Store checksum in firmware header or separate sector

Authenticity (mandatory)

- RSA/ECDSA digital signature
- Public certificate stored in Bootloader I
- Verify signature before flashing

## Conditions for Entering Update Mode

- GPIO state (e.g., button held during reset)
- Flag in backup RAM / RTC
- Host command (via UART, USB, CAN)
- Failed application validation

## STM32 Tools and Libraries

- STM32CubeProgrammer – for flashing and testing
- STM32Cube HAL/LL – for FLASH, CRC, communication
- mbedTLS – for signature and certificate handling (if you go the security route)

## Things to Consider

- Do you want rollback support (revert to previous firmware)?
- Should updates be secure (e.g., encrypted)?
- Can the application initiate an update?
- Do you want diagnostic logs from the update process?

## Platform Abstraction via Concepts

```cpp
template<typename T>
concept FlashDriver = requires(T driver, std::span<const std::byte> data, std::uintptr_t address) {
    { driver.erase(address) } -> std::same_as<bool>;
    { driver.write(address, data) } -> std::same_as<bool>;
};
```

Then implement FlashDriver for STM32, ESP32, Nordic — all using direct registers.

## Flash Memory Map (STM32F4 Example)

depreciated

## Bootloader Architecture Overview

| Layer       | Role                                                                 |
|-------------|----------------------------------------------------------------------|
| Bootloader/ | High-level logic: metadata handling, update orchestration, jump control |
| Platform/   | Hardware-specific implementations: flash access, VTOR config, sector layout |
| BootPrim/   | Minimal bootloader: sets VTOR, verifies BootSec, jumps to it         |
| BootSec/    | Full-featured bootloader: validates firmware, applies update         |

## New Aproach to Flash Layout 23.10.2025

### Firmware update mode

- Per-vendor certificate
- Metadata + certificate embedded at the end of each application
- Sector-aware layout to avoid erase conflicts
- Combine_bin packaging for monolithic image generation

### Flash Layout

+-------------------------------------------------+ 0x08000000
| PRIMARY BOOTLOADER AREA                         | 16 KB (Sector 0)
| - Fixed, minimal startup code                   |
| - Initializes system and jumps to Bootloader2   |
+-------------------------------------------------+ 0x08004000
| PRIVATE CERTIFICATE AREA                        | 16 KB (Sector 1)
| - Stores device-unique or private certificate   |
+-------------------------------------------------+ 0x08008000
| ERROR LOG AREA                                  | 16 KB (Sector 2)
| - Boot and runtime error logs                   |
+-------------------------------------------------+ 0x0800C000
| CONFIGURATION / BOOT FLAGS                      | 16 KB (Sector 3)
| - Boot control flags, configuration data        |
+-------------------------------------------------+ 0x08010000
| SECONDARY BOOTLOADER                            | 64 KB (Sector 4)
| - Main bootloader logic                         |
| - Validates, decrypts, and updates firmware     |
+-------------------------------------------------+ 0x08020000
| MAIN APPLICATION                                | 383 KB (Sectors 5–7 minus 1 KB)
| - Main operational firmware                     |
| - Handles OTA updates and main features         |
|                                                 |
|   ├─ APP METADATA                               | 512 B @ 0x0807FC00–0x0807FDFF
|   │  - Version, CRC, flags                      |
|   └─ APP CERTIFICATE                            | 512 B @ 0x0807FE00–0x0807FFFF
|      - Signature or verification key            |
+-------------------------------------------------+ 0x08090000
| NEW SECONDARY BOOTLOADER                        | 64 KB (Sector 8 upper half)
| - Updated Bootloader2 image (staging area)      |
+-------------------------------------------------+ 0x080A0000
| NEW APPLICATION                                 | 383 KB (Sectors 9–11 minus 1 KB)
| - Staged firmware image                         |
|                                                 |
|   ├─ NEW APP METADATA                           | 512 B @ 0x080FFC00–0x080FFDFF
|   │  - Version, CRC, flags                      |
|   └─ NEW APP CERTIFICATE                        | 512 B @ 0x080FFE00–0x080FFFFF
|      - Signature or verification key            |
+-------------------------------------------------+ 0x080FFFFF

### Bootloader Logic to Verify Hash + Certificate

#### Read Metadata

```cpp
auto metadata = reinterpret_cast<const FirmwareMetadata*>(FlashLayout::APP_METADATA_START);

```

### Validate Magic and Size

```cpp
bool isValidMetadata =
    metadata->magic == Firmware::METADATA_MAGIC &&
    metadata->firmwareSize > 0 &&
    metadata->firmwareSize <= FlashLayout::APP_SIZE;

```

#### Compute SHA-256 of firmware region (Use your crypto library (e.g. mbedTLS, tinycrypt))

```cpp
std::uint8_t computedHash[32];
sha256_compute(reinterpret_cast<const std::uint8_t*>(FlashLayout::APP_START),
               metadata->firmwareSize,
               computedHash);
```

#### Compare hashes

```cpp
bool hashMatch = std::memcmp(computedHash, metadata->firmwareHash, 32) == 0;
```

#### Verify signature (optional for now)

```cpp
bool signatureValid = verify_signature(metadata->firmwareHash, cert, publicKey);
```

#### Jump to application if valid

```cpp
extern const Firmware::Metadata* metadata = reinterpret_cast<const Firmware::Metadata*>(__metadata_start__);

if (hashMatch)
{
    JumpToApp(); // Set VTOR, MSP, and call reset vector
}else
{// Log error, stay in bootloader, or rollback}
```
