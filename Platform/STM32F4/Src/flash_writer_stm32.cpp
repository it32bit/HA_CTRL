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
#include <span>
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

void FlashWriterSTM32F4::lock()
{
    FLASH->CR |= FLASH_CR_LOCK;
}

void FlashWriterSTM32F4::eraseSector(std::uint8_t t_sector)
{
    unlock();

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

    lock();
}

__attribute__((section(".ramfunc"))) void FlashWriterSTM32F4::writeWord(std::uintptr_t t_address,
                                                                        std::uint32_t  t_data)
{
    unlock();

    // Wait for no ongoing operation
    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    // Clear all error flags
    FLASH->SR = FLASH_SR_EOP | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR;

    // Set programming parallelism = 32 bits
    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= FLASH_CR_PSIZE_1; // 10b = 32-bit word programming

    // Enable programming
    FLASH->CR |= FLASH_CR_PG;

    // Write the data
    *(__IO uint32_t*)t_address = t_data;

    // Wait until done
    while (FLASH->SR & FLASH_SR_BSY)
    {
    }

    // Clear EOP flag (optional)
    FLASH->SR = FLASH_SR_EOP;

    // Clear PG bit
    FLASH->CR &= ~FLASH_CR_PG;

    lock();
}

std::uint32_t FlashWriterSTM32F4::makeWord(std::span<const std::byte> bytes)
{
    // Check if bytes are less or equal 4 byte
    // assert(bytes.size() <= sizeof(std::uint32_t));

    // Create 4 byte buffer filled by 0xFF
    std::array<std::byte, sizeof(std::uint32_t)> buf{std::byte{0xFF}, std::byte{0xFF},
                                                     std::byte{0xFF}, std::byte{0xFF}};
    std::copy(bytes.begin(), bytes.end(), buf.begin());

    // Conversion to uint32_t
    return std::bit_cast<std::uint32_t>(buf);
}

void FlashWriterSTM32F4::writeImage(std::uintptr_t t_address_src, std::uintptr_t t_address_dst,
                                    std::size_t t_byte_number)
{
    // assert((t_address_dst % 4) == 0); // destination address must be alligned to word

    // Creation of view to source as a byte
    auto src_bytes =
        std::span<const std::byte>(std::bit_cast<const std::byte*>(t_address_src), t_byte_number);

    const std::size_t full_words = src_bytes.size() / sizeof(std::uint32_t);
    const std::size_t remainder  = src_bytes.size() % sizeof(std::uint32_t);

    // Write full words
    for (std::size_t i = 0; i < full_words; ++i)
    {
        auto word_bytes = src_bytes.subspan(i * sizeof(std::uint32_t), sizeof(std::uint32_t));
        const std::uint32_t word = makeWord(word_bytes);
        writeWord(t_address_dst + i * sizeof(std::uint32_t), word);
    }

    // Last word
    if (remainder != 0)
    {
        auto tail_bytes = src_bytes.subspan(full_words * sizeof(std::uint32_t), remainder);
        const std::uint32_t last_word = makeWord(tail_bytes);

        writeWord(t_address_dst + full_words * sizeof(std::uint32_t), last_word);
    }
}
