#include <stdint-gcc.h>
#include <cstddef>
#include "stm32f4xx_ll_gpio.h" // IWYU pragma: keep
#include "gpio_hal_stm32.hpp"
#include "pil_pin_config.hpp"

static bool clockEnable(const GPIO_TypeDef* port);
static bool gpioHalConfigInterrupt(const PinConfig& t_iodef);

// clang-format off
inline IRQn_Type exti_IRQ_0(void){ return EXTI0_IRQn; }
inline IRQn_Type exti_IRQ_1(void){ return EXTI1_IRQn; }
inline IRQn_Type exti_IRQ_2(void){ return EXTI2_IRQn; }
inline IRQn_Type exti_IRQ_3(void){ return EXTI3_IRQn; }
inline IRQn_Type exti_IRQ_4(void){ return EXTI4_IRQn; }
inline IRQn_Type exti_IRQ_9_5(void){ return EXTI9_5_IRQn; }
inline IRQn_Type exti_IRQ_15_10(void){ return EXTI15_10_IRQn; }

// clang-format on
using irqType = IRQn_Type (*)(void);

constexpr size_t CONST_EXTI_IRQ_PIN_MAX = 16;

inline std::array<irqType, CONST_EXTI_IRQ_PIN_MAX> getExtiIrqFromPin = {
    exti_IRQ_0,     exti_IRQ_1,     exti_IRQ_2,     exti_IRQ_3,    exti_IRQ_4,     exti_IRQ_9_5,
    exti_IRQ_9_5,   exti_IRQ_9_5,   exti_IRQ_9_5,   exti_IRQ_9_5,  exti_IRQ_15_10, exti_IRQ_15_10,
    exti_IRQ_15_10, exti_IRQ_15_10, exti_IRQ_15_10, exti_IRQ_15_10};



/**
 * @brief Lower-level gpioHalConfig(def) function to handle individual GPIO setup.
 */


bool gpioHalConfig(const PinConfig& t_iodef)
{
    constexpr size_t CONST_AFP_PIN_0_7_IS_LOWER = 8;
    
    GPIO_TypeDef* port = getPortStm32FromIndex(t_iodef.portIndex);

    // Validate port and pin number
    if (port == nullptr || t_iodef.pinNumber > HAL_GPIO_PIN_SHIFT_MAX)
    {
        return false;
    }

    // Enable clock for the port
    if (clockEnable(port) == false)
    {
        return false;
    }

    LL_GPIO_SetPinSpeed(port, getGpioPinMask(t_iodef.pinNumber), t_iodef.speed);
    LL_GPIO_SetPinOutputType(port, getGpioPinMask(t_iodef.pinNumber), t_iodef.type);
    LL_GPIO_SetPinPull(port, getGpioPinMask(t_iodef.pinNumber), t_iodef.pull);
    LL_GPIO_SetPinMode(port, getGpioPinMask(t_iodef.pinNumber), t_iodef.mode);

    if ((t_iodef.mode == PinConfig::Mode::Alternate) && (t_iodef.altFunction != 0))
    {
        if (t_iodef.pinNumber < CONST_AFP_PIN_0_7_IS_LOWER)
        {
            LL_GPIO_SetAFPin_0_7(port, getGpioPinMask(t_iodef.pinNumber), t_iodef.altFunction);
        }
        else
        {
            LL_GPIO_SetAFPin_8_15(port, getGpioPinMask(t_iodef.pinNumber), t_iodef.altFunction);
        }
    }
    else if ((t_iodef.mode == PinConfig::Mode::Input) &&
             (t_iodef.iExti == PinConfig::InterruptExti::ExtiIT ||
              t_iodef.iExti == PinConfig::InterruptExti::ExtiEVT))
    {
        auto irq = getExtiIrqFromPin[static_cast<std::size_t>(t_iodef.pinNumber)]();

        __HAL_RCC_SYSCFG_CLK_ENABLE(); // NOLINT

        if (gpioHalConfigInterrupt(t_iodef) == false)
        {
            return false;
        }
        NVIC_ClearPendingIRQ(irq);
        NVIC_SetPriority(irq, t_iodef.iPriority);
        NVIC_EnableIRQ(irq);
    }
    return true;
}

/**
 * @brief Configures external interrupt for a given GPIO pin.
 *
 * This function sets up the EXTI (External Interrupt) line for the specified GPIO pin,
 * including trigger edge selection and NVIC priority configuration. It enables the SYSCFG clock,
 * configures the EXTI mask registers, and maps the pin number to the appropriate IRQ line.
 *
 * @param t_iodef Reference to the PinConfig structure containing GPIO pin configuration.
 *
 * @return true if configuration was successful.
 *
 * @note This function assumes that the pin number is in the range 0–15 and
 *       that the EXTI IRQ mapping is handled via the getExtiIrqFromPin lookup array.
 *
 * @see getExtiIrqFromPin
 * @see getGpioPinMask
 *
 * @note NOLINT: warning: avoid integer to pointer casts [performance-no-int-to-ptr]
 */
static bool gpioHalConfigInterrupt(const PinConfig& t_iodef)
{
    EXTI->EMR &= ~(getGpioPinMask(t_iodef.pinNumber)); // Disable Event NOLINT
    EXTI->IMR |= getGpioPinMask(t_iodef.pinNumber);    // Enable IT     NOLINT

    if (t_iodef.iExti == PinConfig::InterruptExti::ExtiIT)
    {
        EXTI->EMR &= ~(getGpioPinMask(t_iodef.pinNumber)); // NOLINT
        EXTI->IMR |= getGpioPinMask(t_iodef.pinNumber);    // NOLINT
    }
    else if (t_iodef.iExti == PinConfig::InterruptExti::ExtiEVT)
    {
        EXTI->IMR &= ~(getGpioPinMask(t_iodef.pinNumber)); // NOLINT
        EXTI->EMR |= getGpioPinMask(t_iodef.pinNumber);    // NOLINT
    }
    else if (t_iodef.iExti == PinConfig::InterruptExti::ExtiNone)
    {
        return false;
    }

    if (t_iodef.iTrigger == PinConfig::InterruptTrigger::Rising)
    {
        EXTI->RTSR |= getGpioPinMask(t_iodef.pinNumber);    // NOLINT
        EXTI->FTSR &= ~(getGpioPinMask(t_iodef.pinNumber)); // NOLINT
    }
    else if (t_iodef.iTrigger == PinConfig::InterruptTrigger::Falling)
    {
        EXTI->RTSR &= ~(getGpioPinMask(t_iodef.pinNumber)); // NOLINT
        EXTI->FTSR |= (getGpioPinMask(t_iodef.pinNumber));  // NOLINT
    }
    else if (t_iodef.iTrigger == PinConfig::InterruptTrigger::RisingFalling)
    {
        EXTI->RTSR |= getGpioPinMask(t_iodef.pinNumber); // NOLINT
        EXTI->FTSR |= getGpioPinMask(t_iodef.pinNumber); // NOLINT
    }
    else
    {
        return false;
    }
    return true;
}

/**
 * @brief Perypheral GPIO Clock Enable
 *
 * @note NOLINT: warning: avoid integer to pointer casts [performance-no-int-to-ptr]
 */
// NOLINTNEXTLINE clang-tidyreadability-function-cognitive-complexity
static bool clockEnable(const GPIO_TypeDef* port)
{
    if (port == GPIOA) // NOLINT
    {
        __HAL_RCC_GPIOA_CLK_ENABLE(); // NOLINT
    }
    else if (port == GPIOB) // NOLINT
    {
        __HAL_RCC_GPIOB_CLK_ENABLE(); // NOLINT
    }
    else if (port == GPIOC) // NOLINT
    {
        __HAL_RCC_GPIOC_CLK_ENABLE(); // NOLINT
    }
    else if (port == GPIOD) // NOLINT
    {
        __HAL_RCC_GPIOD_CLK_ENABLE(); // NOLINT
    }
    else if (port == GPIOE) // NOLINT
    {
        __HAL_RCC_GPIOE_CLK_ENABLE(); // NOLINT
    }
    else if (port == GPIOF) // NOLINT
    {
        __HAL_RCC_GPIOF_CLK_ENABLE(); // NOLINT
    }
    else if (port == GPIOG) // NOLINT
    {
        __HAL_RCC_GPIOG_CLK_ENABLE(); // NOLINT
    }
    else if (port == GPIOH) // NOLINT
    {
        __HAL_RCC_GPIOH_CLK_ENABLE(); // NOLINT
    }
    else if (port == GPIOI) // NOLINT
    {
        __HAL_RCC_GPIOI_CLK_ENABLE(); // NOLINT
    }
    else
    {
        return false;
    }

    return true;
}
