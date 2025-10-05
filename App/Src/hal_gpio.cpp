#include "hal_gpio.hpp"
#include <stdint-gcc.h>
#include <cstddef>

static bool clockEnable(const GPIO_TypeDef* port);

static bool configGpioInterrupt(const IOD& t_io);

/**
 * @section Inline hal_gpio
 */
inline void setOutputNoop(GPIO_TypeDef* gpio, uint32_t pin) {}

inline void setOutputLow(GPIO_TypeDef* gpio, uint32_t pin)
{
    LL_GPIO_ResetOutputPin(gpio, pin);
}

inline void setOutputHigh(GPIO_TypeDef* gpio, uint32_t pin)
{
    LL_GPIO_SetOutputPin(gpio, pin);
}

using gpioAction = void (*)(GPIO_TypeDef*, uint32_t);

/** IOD -> enum ISTATE : uint32_t {DONT_CARE,     LOGIC_LOW,    LOGIC_HIGH} */
inline std::array<gpioAction, 3> setGpioAction = {setOutputNoop, setOutputLow, setOutputHigh};

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
 * @brief Lower-level gpioConfig(def) function to handle individual GPIO setup.
 */
constexpr size_t CONST_AFP_PIN_0_7_IS_LOWER = 8;

bool gpioConfig(const IOD& t_io)
{
    clockEnable(t_io.GPIO);

    LL_GPIO_SetPinSpeed(t_io.GPIO, getGpioPinMask(t_io.PinNb), t_io.Speed);
    LL_GPIO_SetPinOutputType(t_io.GPIO, getGpioPinMask(t_io.PinNb), t_io.Type);
    LL_GPIO_SetPinPull(t_io.GPIO, getGpioPinMask(t_io.PinNb), t_io.Pupdr);
    LL_GPIO_SetPinMode(t_io.GPIO, getGpioPinMask(t_io.PinNb), t_io.Moder);

    if (t_io.Moder == IOD::MODER::OUTPUT)
    {
        setGpioAction[static_cast<std::size_t>(t_io.InitState)](t_io.GPIO,
                                                                getGpioPinMask(t_io.PinNb));
    }
    else if (t_io.Moder == IOD::MODER::ALT)
    {
        if (t_io.PinNb < CONST_AFP_PIN_0_7_IS_LOWER)
        {
            LL_GPIO_SetAFPin_0_7(t_io.GPIO, getGpioPinMask(t_io.PinNb), t_io.AltFunc);
        }
        else
        {
            LL_GPIO_SetAFPin_8_15(t_io.GPIO, getGpioPinMask(t_io.PinNb), t_io.AltFunc);
        }
    }
    else if ((t_io.Moder == IOD::MODER::INPUT) && (t_io.Exti == IOD::GEXTI::IT))
    {
        auto irq = getExtiIrqFromPin[static_cast<std::size_t>(t_io.PinNb)]();

        __HAL_RCC_SYSCFG_CLK_ENABLE(); // NOLINT

        configGpioInterrupt(t_io);
        NVIC_ClearPendingIRQ(irq);
        NVIC_SetPriority(irq, t_io.IPriority);
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
 * @param io       Reference to the IOD structure containing GPIO pin configuration.
 * @param priority NVIC priority level for the corresponding EXTI interrupt.
 *
 * @return true if configuration was successful.
 *
 * @note This function assumes that the pin number (io.PinNb) is in the range 0–15 and
 *       that the EXTI IRQ mapping is handled via the getExtiIrqFromPin lookup array.
 *
 * @see getExtiIrqFromPin
 * @see getGpioPinMask
 *
 * @note NOLINT: warning: avoid integer to pointer casts [performance-no-int-to-ptr]
 */
static bool configGpioInterrupt(const IOD& t_io)
{
    EXTI->EMR &= ~(getGpioPinMask(t_io.PinNb)); // Disable Event NOLINT
    EXTI->IMR |= getGpioPinMask(t_io.PinNb);    // Enable IT     NOLINT

    if (t_io.Exti == IOD::GEXTI::IT)
    {
        EXTI->EMR &= ~(getGpioPinMask(t_io.PinNb)); // NOLINT
        EXTI->IMR |= getGpioPinMask(t_io.PinNb);    // NOLINT
    }
    else if (t_io.Exti == IOD::GEXTI::EVT)
    {
        EXTI->IMR &= ~(getGpioPinMask(t_io.PinNb)); // NOLINT
        EXTI->EMR |= getGpioPinMask(t_io.PinNb);    // NOLINT
    }
    else if (t_io.Exti == IOD::GEXTI::NONE)
    {
        return false;
    }

    if (t_io.Trg == IOD::ITRG::RISING)
    {
        EXTI->RTSR |= getGpioPinMask(t_io.PinNb);    // NOLINT
        EXTI->FTSR &= ~(getGpioPinMask(t_io.PinNb)); // NOLINT
    }
    else if (t_io.Trg == IOD::ITRG::FALLING)
    {
        EXTI->RTSR &= ~(getGpioPinMask(t_io.PinNb)); // NOLINT
        EXTI->FTSR |= (getGpioPinMask(t_io.PinNb));  // NOLINT
    }
    else if (t_io.Trg == IOD::ITRG::RIS_FALL)
    {
        EXTI->RTSR |= getGpioPinMask(t_io.PinNb); // NOLINT
        EXTI->FTSR |= getGpioPinMask(t_io.PinNb); // NOLINT
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
static bool
clockEnable(const GPIO_TypeDef* port) // NOLINT clang-tidyreadability-function-cognitive-complexity
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
