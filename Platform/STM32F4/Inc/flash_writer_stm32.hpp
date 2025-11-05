/**
 * @file      Platform/STM32/Src/flash_writer_stm32.hpp
 * @author    it32bit
 * @brief     Flash writer interface implementation for STM32F4 platform.
 *            Provides low-level flash operations such as unlocking,
 *            sector erase, and 32-bit word programming.
 *
 * @version   1.0
 * @date      2025-10-24
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef FLASH_WRITER_HPP
#define FLASH_WRITER_HPP

#include <span>
#include "pil_flash_writer.hpp"

class FlashWriterSTM32F4 : public IFlashWriter
{
  public:
    void eraseSector(std::uint8_t t_sector) override;
    void writeImage(std::uintptr_t t_address_src, std::uintptr_t t_address_dst,
                    std::size_t t_wordCount) override;

    __attribute__((section(".ramfunc"))) void writeWord(std::uintptr_t t_address,
                                                        std::uint32_t  t_data) override;

  private:
    void unlock();
    void lock();

    std::uint32_t makeWord(std::span<const std::byte> bytes);
};

#endif // FLASH_WRITER_HPP
