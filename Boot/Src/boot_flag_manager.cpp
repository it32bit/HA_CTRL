#include "boot_flag_manager.hpp"
#include "stm32f4xx.h"
#include "flash_layout.hpp"

BootFlagManager::BootFlagManager(IFlashWriter* writer) : m_writer(writer) {}

BootState BootFlagManager::getState() const
{
    return static_cast<BootState>(*reinterpret_cast<volatile std::uint32_t*>(FLAG_ADDR));
}

void BootFlagManager::setState(BootState state)
{
    __disable_irq();

    m_writer->unlock();
    m_writer->eraseSector(CONFIG_SECTOR); // Define CONFIG_SECTOR based on address
    m_writer->writeWord(FLAG_ADDR, static_cast<std::uint32_t>(state));

    __enable_irq();

    uint32_t readBack = *(__IO uint32_t*)FLAG_ADDR;
    if (readBack != static_cast<uint32_t>(state))
    {
        // handle error or re-erase/retry
    }
}

void BootFlagManager::clear()
{
    setState(BootState::Idle);
}
