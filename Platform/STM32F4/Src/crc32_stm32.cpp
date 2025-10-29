#include "crc32_stm32.hpp"
#include "stm32f4xx.h"
#include "stdint-gcc.h"

std::uint32_t CRC32Hardware::compute(const std::uint8_t* t_data, std::size_t t_length)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
    CRC->CR = CRC_CR_RESET;

    std::size_t i = 0;
    while (i + 4 <= t_length)
    {
        std::uint32_t word = (static_cast<std::uint32_t>(t_data[i]) << 24) |
                             (static_cast<std::uint32_t>(t_data[i + 1]) << 16) |
                             (static_cast<std::uint32_t>(t_data[i + 2]) << 8) |
                             (static_cast<std::uint32_t>(t_data[i + 3]));
        CRC->DR = word;
        i += 4;
    }

    if (i < t_length)
    {
        std::uint32_t last = 0;
        for (std::size_t j = 0; j < t_length - i; ++j)
        {
            last |= static_cast<std::uint32_t>(t_data[i + j]) << (24 - j * 8);
        }
        CRC->DR = last;
    }

    return CRC->DR;
}
