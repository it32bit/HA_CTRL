/**
 ******************************************************************************
 * @file        flash_writer_stm32.cpp
 * @author      it32bit
 * @brief       Low-level Flash Writer implementation for STM32F4 devices.
 *
 *              Provides functions for unlocking, erasing, and programming the
 *              internal Flash memory on STM32F4 microcontrollers. Operations
 *              are performed directly through the FLASH peripheral registers
 *              for maximum control and efficiency.
 *
 * @details     - Implements sector erase and word write functionality
 *              - The writeWord() function is placed in RAM (.ramfunc section)
 *                to allow flash programming while executing outside of flash
 *              - Error flags are cleared before each operation to ensure safe
 *                and consistent write behavior
 *              - All operations block until completion (polling mode)
 *
 * @note        - Compatible with STM32F407 and similar STM32F4-series MCUs
 *              - Flash operations must not be interrupted
 *              - Ensure interrupts and cache are disabled during write/erase
 *
 * @license     MIT License
 *              This software is provided "as is", without warranty of any kind.
 *              The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
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

__attribute__((section(".ramfunc"))) void FlashWriterSTM32F4::writeWord(std::uintptr_t t_address,
                                                                        std::uint32_t  t_data)
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
    *(__IO uint32_t*)t_address = 0x53544147; //t_data;

    // Wait until done
    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    // Clear PG bit
    FLASH->CR &= ~FLASH_CR_PG;
}
