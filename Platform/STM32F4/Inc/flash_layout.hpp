#ifndef FLASH_LAYOUT_HPP
#define FLASH_LAYOUT_HPP

#include <cstdint>

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
namespace FlashLayout
{
constexpr std::uintptr_t FLASH_BASE_ADDR  = 0x08000000;
constexpr std::size_t    FLASH_TOTAL_SIZE = 1024 * 1024; // 1 MB

// Primary Region
constexpr std::uintptr_t BOOTLOADER1_START = 0x08000000;
constexpr std::size_t    BOOTLOADER1_SIZE  = 16 * 1024;

constexpr std::uintptr_t BOOTLOADER2_START = 0x08004000;
constexpr std::size_t    BOOTLOADER2_SIZE  = 48 * 1024;

constexpr std::uintptr_t APP_START = 0x0800C000;
constexpr std::size_t    APP_SIZE  = 448 * 1024;

// Secondary Region
constexpr std::uintptr_t NEW_APP_START = 0x08080000;
constexpr std::size_t    NEW_APP_SIZE  = 448 * 1024;

constexpr std::uintptr_t METADATA_START = 0x080F8000;
constexpr std::size_t    METADATA_SIZE  = 1 * 1024;

constexpr std::uintptr_t CERT_PUBLIC_START = 0x080F8400;
constexpr std::size_t    CERT_PUBLIC_SIZE  = 1 * 1024;

constexpr std::uintptr_t RESERVED_ALIGN_START = 0x080F8800;
constexpr std::size_t    RESERVED_ALIGN_SIZE  = 1 * 1024;

constexpr std::uintptr_t NEW_BOOTLOADER2_START = 0x080F8C00;
constexpr std::size_t    NEW_BOOTLOADER2_SIZE  = 48 * 1024;

constexpr std::uintptr_t CONFIG_START = 0x080FA800;
constexpr std::size_t    CONFIG_SIZE  = 2 * 1024;

constexpr std::uintptr_t RESERVED_AREA_START = 0x080FAA00;
constexpr std::size_t    RESERVED_AREA_SIZE  = 6 * 1024;

constexpr std::uintptr_t CERT_PRIVATE_START = 0x080FC000;
constexpr std::size_t    CERT_PRIVATE_SIZE  = 1 * 1024;

constexpr std::uintptr_t ERROR_LOG_START = 0x080FC400;
constexpr std::size_t    ERROR_LOG_SIZE  = 4 * 1024;

// Flash End
constexpr std::uintptr_t FLASH_END_ADDR = 0x080FFFFF;
} // namespace FlashLayout

#endif // FLASH_LAYOUT_HPP
