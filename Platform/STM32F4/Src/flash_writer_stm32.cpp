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

    // Clear error flags
    FLASH->SR |= FLASH_SR_EOP | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR;

    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= FLASH_CR_PSIZE_1; // 32-bit programming

    FLASH->CR &= ~FLASH_CR_SNB;
    FLASH->CR |= FLASH_CR_SER | (t_sector << FLASH_CR_SNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    FLASH->CR &= ~FLASH_CR_SER;
}

__attribute__((section(".ramfunc")))
void FlashWriterSTM32F4::writeWord(std::uintptr_t t_address, std::uint32_t t_data)
{
    // Wait for no ongoing operation
    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    // Clear all error flags
    FLASH->SR |= FLASH_SR_EOP | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR;

    // Set programming parallelism = 32 bits
    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= FLASH_CR_PSIZE_1; // 10b = 32-bit word programming

    // Enable programming
    FLASH->CR |= FLASH_CR_PG;

    // Write the data
    *(__IO uint32_t*)t_address = 0x53544147;//t_data;

    // Wait until done
    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    // Clear PG bit
    FLASH->CR &= ~FLASH_CR_PG;
}
