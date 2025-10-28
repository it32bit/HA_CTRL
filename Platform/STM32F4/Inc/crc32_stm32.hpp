#ifndef CRC32_STM32_HPP
#define CRC32_STM32_HPP

#include <cstdint>
#include <cstddef>

class CRC32Hardware
{
  public:
    static std::uint32_t compute(const std::uint8_t* data, std::size_t length);
};

#endif // CRC32_STM32_HPP