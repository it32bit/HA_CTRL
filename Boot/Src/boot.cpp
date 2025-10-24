/**
 * @file      BootCore/Src/boot.cpp
 * @author    it32bit
 * @brief     Bootloader core logic for firmware update management.
 *            Handles copying of the new firmware image from the update
 *            region to the main application area using low-level flash access.
 *
 * @version   1.0
 * @date      2025-10-24
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include <cstdint>
#include <cstring>
#include <span>

#include "boot.hpp"
#include "flash_layout.hpp"

Bootloader::Bootloader(IFlashWriter* writer) : m_flash(writer) {}

void Bootloader::applyUpdate()
{
    const auto& meta = m_metadata();

    auto src = std::span<const std::uint8_t>(
        reinterpret_cast<const std::uint8_t*>(FlashLayout::NEW_APP_START), meta.firmwareSize);

    std::uintptr_t dst = FlashLayout::APP_START;

    m_flash->unlock();
    m_flash->eraseSector(5); // Sector for APP_START

    for (std::size_t i = 0; i < src.size(); i += 4)
    {
        std::uint32_t word = 0;
        std::memcpy(&word, src.data() + i, sizeof(word));
        m_flash->writeWord(dst + i, word);
    }
}
