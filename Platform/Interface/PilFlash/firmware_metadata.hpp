// firmware_metadata.hpp
#ifndef FIRMWARE_METADATA_HPP
#define FIRMWARE_METADATA_HPP

#include <cstdint>

// Should be placed at METADATA_START and use it in both Secendary Bootloader and App.

namespace Firmware
{
constexpr uint32_t METADATA_MAGIC = 0xDEADBEEF;

// Total size: 512 bytes (with padding)
// Place this at APP_METADATA_START or NEW_APP_METADATA_START
// Hash should cover APP_START to APP_METADATA_START - 1
struct Metadata
{
    std::uint32_t magic;
    std::uint32_t version;          // Semantic version encoded as 0xMMmmpp (Major.Minor.Patch)
    std::uint32_t buildTimestamp;   // Unix timestamp of build time
    std::uint32_t firmwareSize;     // Size of firmware in bytes (excluding metadata + cert)
    std::uint32_t firmwareCRC;      // CRC32 of firmware region
    std::uint32_t reserved[3];      // Reserved for future use or alignment
    std::uint8_t  firmwareHash[32]; // SHA-256 hash of firmware region

} __attribute__((packed));

} // namespace Firmware

#endif // FIRMWARE_METADATA_HPP