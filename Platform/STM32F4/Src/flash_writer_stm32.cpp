// flash_writer_stm32.cpp
#include <cstdint>
#include "stm32f4xx.h"
#include "flash_writer_stm32.hpp"

void FlashWriterSTM32F4::unlock()
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }
}

void FlashWriterSTM32F4::eraseSector(std::uint8_t t_sector)
{
    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    FLASH->CR &= ~FLASH_CR_SNB;
    FLASH->CR |= FLASH_CR_SER | (t_sector << FLASH_CR_SNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    FLASH->CR &= ~FLASH_CR_SER;
}

void FlashWriterSTM32F4::writeWord(std::uintptr_t t_address, std::uint32_t t_data)
{
    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    FLASH->CR |= FLASH_CR_PG;
    *reinterpret_cast<volatile std::uint32_t*>(t_address) = t_data;

    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    FLASH->CR &= ~FLASH_CR_PG;
}
