#ifndef CRC32_CHECK_HPP
#define CRC32_CHECK_HPP

#include <cstdint>
#include <cstddef>
#include <span>

namespace Integrity
{

class CRC32Checker
{
  public:
    static std::uint32_t compute(std::span<const std::uint8_t> t_data);

    static bool verify(std::span<const std::uint8_t> t_data, std::uint32_t expected_crc);
};

} // namespace Integrity

#endif // CRC32_CHECK_HPP
