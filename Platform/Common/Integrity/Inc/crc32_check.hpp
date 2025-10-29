#ifndef CRC32_CHECK_HPP
#define CRC32_CHECK_HPP

#include <cstdint>
#include <cstddef>

namespace Integrity
{

class CRC32Checker
{
  public:
    static std::uint32_t compute(const std::uint8_t* t_data, std::size_t t_length);

    static bool verify(const std::uint8_t* t_data, std::size_t t_length,
                       std::uint32_t t_expected_crc);
};

} // namespace Integrity

#endif // CRC32_CHECK_HPP
