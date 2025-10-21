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
        reinterpret_cast<const std::uint8_t*>(FlashLayout::NEW_APP_START), meta.size);

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
