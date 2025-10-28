#include "crc32_check.hpp"
#include "crc32_stm32.hpp"

namespace Integrity
{

// Polynomial: 0xEDB88320 (standard Ethernet, ZIP, PNG)

std::uint32_t CRC32Checker::compute(const std::uint8_t* data, std::size_t length)
{
    return CRC32Hardware::compute(data, length);
}

bool CRC32Checker::verify(const std::uint8_t* data, std::size_t length, std::uint32_t expected_crc)
{
    uint32_t crc32 = compute(data, length);
    return crc32 == expected_crc;
}

} // namespace Integrity
