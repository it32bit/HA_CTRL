#include "boot_flag_manager.hpp"
#include "stm32f4xx.h"
#include "flash_layout.hpp"
#include "image_manager.hpp"

// TODO: To be removed, for debug purpose
static void debugLedBlue()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    GPIOD->MODER &= ~(0b11 << (15 * 2)); // Clear mode
    GPIOD->MODER |= (0b01 << (15 * 2));  // Set to output
    GPIOD->OTYPER &= ~(1 << 15);
    GPIOD->OSPEEDR |= (0b11 << (15 * 2));
    GPIOD->PUPDR &= ~(0b11 << (15 * 2));
    GPIOD->ODR |= (1 << 15);
}

BootFlagManager::BootFlagManager(IFlashWriter* writer) : m_writer(writer) {}

BootState BootFlagManager::getState() const
{
    return static_cast<BootState>(*reinterpret_cast<volatile std::uint32_t*>(FLAG_ADDR));
}

void BootFlagManager::setState(BootState state)
{
    CriticalSection criticalSection; // interrupts disabled here

    m_writer->eraseSector(CONFIG_SECTOR); // Define CONFIG_SECTOR based on address
    m_writer->writeWord(FLAG_ADDR, static_cast<std::uint32_t>(state));

    uint32_t readBack = *(__IO uint32_t*)FLAG_ADDR;
    if (readBack != static_cast<uint32_t>(state))
    {
        debugLedBlue();
    }
}

void BootFlagManager::clear()
{
    setState(BootState::Idle);
}
