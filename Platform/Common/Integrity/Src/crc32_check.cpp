#include "crc32_check.hpp"
#include "crc32_stm32.hpp"
#include <span>

namespace Integrity
{

// Polynomial: 0xEDB88320 (standard Ethernet, ZIP, PNG)
// Polynomial: 0x04C11DB7 (STM hardcoded)

std::uint32_t CRC32Checker::compute(std::span<const std::uint8_t> t_data)
{
    return CRC32Hardware::compute(t_data.data(), t_data.size());
}

bool CRC32Checker::verify(std::span<const std::uint8_t> t_data, std::uint32_t t_expected_crc)
{
    uint32_t crc32 = compute(t_data);
    return crc32 == t_expected_crc;
}

} // namespace Integrity
