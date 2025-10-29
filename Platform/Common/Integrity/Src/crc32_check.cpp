#include "crc32_check.hpp"
#include "crc32_stm32.hpp"

namespace Integrity
{

// Polynomial: 0xEDB88320 (standard Ethernet, ZIP, PNG)

std::uint32_t CRC32Checker::compute(const std::uint8_t* t_data, std::size_t t_length)
{
    return CRC32Hardware::compute(t_data, t_length);
}

bool CRC32Checker::verify(const std::uint8_t* t_data, std::size_t t_length, std::uint32_t t_expected_crc)
{
    uint32_t crc32 = compute(t_data, t_length);
    return crc32 == t_expected_crc;
}

} // namespace Integrity
