/**
 ******************************************************************************
 * @file        gpio_manager_stm32.hpp
 * @author      it32bit
 * @brief       GPIO Manager for the STM32 platform.
 *
 *              Provides centralized initialization and access to GPIO pins using
 *              compile-time PinId mapping and platform-specific configuration.
 *              Designed for static memory usage and zero runtime allocation.
 *
 * @note        - Preallocated fixed-size arrays
 *              - PinId-indexed access
 *              - No dynamic memory allocation
 *
 * @license     MIT License
 *              This software is provided "as is", without warranty of any kind.
 *              The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#ifndef _GPIO_MANAGER_STM32_HPP_
#define _GPIO_MANAGER_STM32_HPP_

#include "pil_gpio.hpp"
#include "pil_pin_config.hpp"
#include "pil_pin_id.hpp"
#include "gpio_config_stm32.hpp"
#include <vector>
#include <memory>
#include <span>

constexpr size_t MAX_PORT_PINS_STM32 = 16; // Arbitrary limit to avoid excessive memory usage

class GpioManager : public IGPIOManager
{
  public:
    constexpr GpioManager() = default;

    void initialize(std::span<const PinConfig> t_configs);

    [[nodiscard]] IGPIOPin* getPin(PinId t_id) override;

    // clang-format off
    void setPin(PinId id) { if (auto p = getPin(id)) p->set(); }
    void resetPin(PinId id) { if (auto p = getPin(id)) p->reset(); }
    void togglePin(PinId id) { if (auto p = getPin(id)) p->toggle(); }

    // clang-format on

    void reset()
    {
        m_pinPtrs.fill(nullptr);
        m_configPtrs.fill(nullptr);
        m_pinCount = 0;
    }

  private:
    std::array<GpioPin_STM32, PIN_CONFIG_ARRAY_SIZE>    m_pinPool{};
    std::array<IGPIOPin*, PIN_CONFIG_ARRAY_SIZE>        m_pinPtrs{};
    std::array<const PinConfig*, PIN_CONFIG_ARRAY_SIZE> m_configPtrs{};
    std::size_t                                         m_pinCount = 0;
};

#endif // _GPIO_MANAGER_STM32_HPP_
