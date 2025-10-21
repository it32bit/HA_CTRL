// firmware_metadata.hpp
#ifndef FIRMWARE_METADATA_HPP
#define FIRMWARE_METADATA_HPP

#include <cstdint>

// Should be placed at METADATA_START and use it in both Secendary Bootloader and App.

namespace Firmware
{
constexpr uint32_t METADATA_MAGIC = 0xDEADBEEF;

struct Metadata
{
    uint32_t magic;        // Must be METADATA_MAGIC
    uint32_t version;      // Semantic version or build ID
    uint32_t size;         // Firmware image size in bytes
    uint32_t crc32;        // CRC32 checksum of firmware
    uint32_t flags;        // Status flags (e.g., update pending, verified)
    uint8_t  reserved[12]; // Padding for future use
} __attribute__((packed));

} // namespace Firmware

#endif // FIRMWARE_METADATA_HPP