

/**
 * std::uintptr_t Typically used when you need to:
 *              Convert a pointer to an integer.
 *              Perform arithmetic or bitwise operations on pointer values.
 *              Store raw pointer addresses in a type-safe, integer-friendly way.
 * Caution:
 * - Always use reinterpret_cast when converting between pointers and uintptr_t
 * - Avoid using it for normal pointer math
 *   it’s meant for special use cases like hardware-level manipulation,
 *   serialization, or hashing.
 */

/**
### STM32F407VGTX Flash Memory Layout

| Sector | Size   | Start Address  | End Address   | Notes                          |
|--------|--------|----------------|---------------|--------------------------------|
| 0      | 16 KB  | 0x0800_0000    | 0x0800_3FFF   | Primary Bootloader             |
| 1      | 16 KB  | 0x0800_4000    | 0x0800_7FFF   | Private Certificate |
| 2      | 16 KB  | 0x0800_8000    | 0x0800_BFFF   | Error Logs                     |
| 3      | 16 KB  | 0x0800_C000    | 0x0800_FFFF   | Boot Flags + Configuration     |
| 4      | 64 KB  | 0x0801_0000    | 0x0801_FFFF   | Secendary Bootloader           |
| 5      | 128 KB | 0x0802_0000    | 0x0803_FFFF   | Main application start         |
| 6      | 128 KB | 0x0804_0000    | 0x0805_FFFF   | Main application               |
| 7      | 128 KB | 0x0806_0000    | 0x0807_FFFF   | Main application end (last 1kB should contain MetaData and Firmware Certificate) |
|-----------------------------------------------------------------------------------|
| 8      | 128 KB | 0x0808_0000    | 0x0808_FFFF   | Reserved space in sector 8     |
|        |        | 0x0809_0000    | 0x0809_FFFF   | New Secendary Bootloader in upper half of sector 8 |
|-----------------------------------------------------------------------------------|
| 9      | 128 KB | 0x080A_0000    | 0x080B_FFFF   | New application start          |
| 10     | 128 KB | 0x080C_0000    | 0x080D_FFFF   | New application                |
| 11     | 128 KB | 0x080E_0000    | 0x080F_FFFF   | New application end (last 1kB should contain MetaData and Firmware Certificate)

*/
#ifndef FLASH_LAYOUT_HPP
#define FLASH_LAYOUT_HPP

#include <cstdint>

namespace FlashLayout
{
// Sector mapping for STM32F407VGTX (1MB flash)
constexpr std::uint8_t sectorFromAddress(std::uintptr_t addr)
{
    if (addr < 0x08004000)
        return 0;
    if (addr < 0x08008000)
        return 1;
    if (addr < 0x0800C000)
        return 2;
    if (addr < 0x08010000)
        return 3;
    if (addr < 0x08020000)
        return 4;
    if (addr < 0x08040000)
        return 5;
    if (addr < 0x08060000)
        return 6;
    if (addr < 0x08080000)
        return 7;
    if (addr < 0x080A0000)
        return 8;
    if (addr < 0x080C0000)
        return 9;
    if (addr < 0x080E0000)
        return 10;
    return 11;
}

// Flash boundaries
constexpr std::uintptr_t FLASH_BASE_ADDR  = 0x08000000;
constexpr std::size_t    FLASH_TOTAL_SIZE = 1024 * 1024;
constexpr std::uintptr_t FLASH_END_ADDR   = 0x080FFFFF;

// -------------------------------------------------------------
// Primary Region
// -------------------------------------------------------------

// Sector 0: Primary Bootloader
constexpr std::uintptr_t BOOTLOADER1_START = 0x08000000;
constexpr std::size_t    BOOTLOADER1_SIZE  = 16 * 1024;

// Sector 1: Reserved for private certificate
constexpr std::uintptr_t CERT_PRIVATE_START = 0x08004000;
constexpr std::size_t    CERT_PRIVATE_SIZE  = 16 * 1024;

// Sector 2: Error logs
constexpr std::uintptr_t ERROR_LOG_START = 0x08008000;
constexpr std::size_t    ERROR_LOG_SIZE  = 16 * 1024;

// Sector 3: Boot flags + configuration
constexpr std::uintptr_t CONFIG_START = 0x0800C000;
constexpr std::size_t    CONFIG_SIZE  = 16 * 1024;

// Sector 4: Secondary Bootloader
constexpr std::uintptr_t BOOTLOADER2_START = 0x08010000;
constexpr std::size_t    BOOTLOADER2_SIZE  = 64 * 1024;

// Sectors 5–7: Main Application
constexpr std::uintptr_t APP_START = 0x08020000;
constexpr std::size_t    APP_TOTAL_SIZE = (128 + 128 + 128) * 1024; // 384KB
constexpr std::size_t    APP_RESERVED_SIZE = 1024; // Last 1KB reserved
constexpr std::size_t    APP_SIZE = APP_TOTAL_SIZE - APP_RESERVED_SIZE;

constexpr std::uintptr_t APP_METADATA_START = 0x0807FC00;
constexpr std::size_t    APP_METADATA_SIZE  = 512;

constexpr std::uintptr_t APP_CERT_START = 0x0807FE00;
constexpr std::size_t    APP_CERT_SIZE  = 512;

// -------------------------------------------------------------
// Update Region
// -------------------------------------------------------------

// Sector 8: Reserved (lower half) + New Secondary Bootloader (upper half)
constexpr std::uintptr_t NEW_BOOTLOADER2_START = 0x08090000;
constexpr std::size_t    NEW_BOOTLOADER2_SIZE  = 64 * 1024;

// Sectors 9–11: New Application
constexpr std::uintptr_t NEW_APP_START = 0x080A0000;
constexpr std::size_t    NEW_APP_TOTAL_SIZE = (128 + 128 + 128) * 1024;
constexpr std::size_t    NEW_APP_RESERVED_SIZE = 1024;
constexpr std::size_t    NEW_APP_SIZE = NEW_APP_TOTAL_SIZE - NEW_APP_RESERVED_SIZE;

constexpr std::uintptr_t NEW_APP_METADATA_START = 0x080FFC00;
constexpr std::size_t    NEW_APP_METADATA_SIZE  = 512;

constexpr std::uintptr_t NEW_APP_CERT_START = 0x080FFE00;
constexpr std::size_t    NEW_APP_CERT_SIZE  = 512;

} // namespace FlashLayout

#endif // FLASH_LAYOUT_HPP
