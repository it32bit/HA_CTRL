/**
 ******************************************************************************
 * @file        gpio_manager_stm32.cpp
 * @author      it32bit
 * @brief       GPIO peripheral dispatcher for STM32 platform.
 *
 *              Implements centralized GPIO initialization and access using
 *              compile-time PinId mapping and static memory allocation.
 *
 * @license     MIT License
 *              This software is provided "as is", without warranty of any kind.
 *              The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#include "gpio_pin_stm32.hpp"
#include "gpio_manager_stm32.hpp"
#include "gpio_hal_stm32.hpp"

void GpioManager::initialize(std::span<const PinConfig> t_configs)
{
    reset();

    for (const auto& cfg : t_configs)
    {
        auto idx = static_cast<std::size_t>(cfg.id);

        if (idx >= m_pinPool.size())
            continue;

        if (cfg.pinNumber >= MAX_PORT_PINS_STM32)
            continue;

        GPIO_TypeDef* port = getPortStm32FromIndex(cfg.portIndex);
        if (!port)
            continue;

        if (!gpioHalConfig(cfg))
            continue;

        m_pinPool[idx]    = GpioPin_STM32(port, cfg.pinNumber);
        m_pinPtrs[idx]    = &m_pinPool[idx];
        m_configPtrs[idx] = &cfg;

        ++m_pinCount;
    }
}

IGPIOPin* GpioManager::getPin(PinId id)
{
    auto idx = static_cast<std::size_t>(id);
    if (idx >= m_pinPtrs.size())
        return nullptr;

    return m_pinPtrs[idx];
}
