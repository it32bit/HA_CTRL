#include "hal_gpio.hpp"
#include "stm32f4xx_ll_exti.h"

static constexpr bool clockEnable(const GPIO_TypeDef* port);

static bool configGpioInterrupt(const IOD& io);

/**
 * @section Inline hal_gpio
 */
inline void setOutputNoop(GPIO_TypeDef*, uint32_t) {}

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

inline std::array<irqType, 16> getExtiIrqFromPin = {
    exti_IRQ_0,     exti_IRQ_1,     exti_IRQ_2,     exti_IRQ_3,    exti_IRQ_4,     exti_IRQ_9_5,
    exti_IRQ_9_5,   exti_IRQ_9_5,   exti_IRQ_9_5,   exti_IRQ_9_5,  exti_IRQ_15_10, exti_IRQ_15_10,
    exti_IRQ_15_10, exti_IRQ_15_10, exti_IRQ_15_10, exti_IRQ_15_10};

/**
 * @brief Lower-level hal_ConfigGpio(def) function to handle individual GPIO setup.
 */
bool hal_ConfigGpio(const IOD& io)
{
    clockEnable(io.GPIO);

    LL_GPIO_SetPinSpeed(io.GPIO, getGpioPinMask(io.PinNb), io.Speed);
    LL_GPIO_SetPinOutputType(io.GPIO, getGpioPinMask(io.PinNb), io.Type);
    LL_GPIO_SetPinPull(io.GPIO, getGpioPinMask(io.PinNb), io.Pupdr);
    LL_GPIO_SetPinMode(io.GPIO, getGpioPinMask(io.PinNb), io.Moder);

    if (io.Moder == IOD::MODER::OUTPUT)
    {
        setGpioAction[static_cast<std::size_t>(io.InitState)](io.GPIO, getGpioPinMask(io.PinNb));
    }
    else if (io.Moder == IOD::MODER::ALT)
    {
        if (io.PinNb < 8u)
        {
            LL_GPIO_SetAFPin_0_7(io.GPIO, getGpioPinMask(io.PinNb), io.AltFunc);
        }
        else
        {
            LL_GPIO_SetAFPin_8_15(io.GPIO, getGpioPinMask(io.PinNb), io.AltFunc);
        }
    }
    else if ((io.Moder == IOD::MODER::INPUT) && (io.Exti == IOD::GEXTI::IT))
    {
        auto irq = getExtiIrqFromPin[static_cast<std::size_t>(io.PinNb)]();
        __HAL_RCC_SYSCFG_CLK_ENABLE();

        configGpioInterrupt(io);
        NVIC_ClearPendingIRQ(irq);
        NVIC_SetPriority(irq, io.IPriority);
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
 */
static bool configGpioInterrupt(const IOD& io)
{
    EXTI->EMR &= ~(getGpioPinMask(io.PinNb)); // Disable Event
    EXTI->IMR |= getGpioPinMask(io.PinNb);    // Enable IT

    if (io.Exti == IOD::GEXTI::IT)
    {
        EXTI->EMR &= ~(getGpioPinMask(io.PinNb)); // Disable Event
        EXTI->IMR |= getGpioPinMask(io.PinNb);    // Enable IT
    }
    else if (io.Exti == IOD::GEXTI::EVT)
    {
        EXTI->IMR &= ~(getGpioPinMask(io.PinNb)); // Disable IT
        EXTI->EMR |= getGpioPinMask(io.PinNb);    // Enable Event
    }
    else if (io.Exti == IOD::GEXTI::NONE)
    {
        return false;
    }

    if (io.Trg == IOD::ITRG::RISING)
    {
        EXTI->RTSR |= getGpioPinMask(io.PinNb);
        EXTI->FTSR &= ~(getGpioPinMask(io.PinNb));
    }
    else if (io.Trg == IOD::ITRG::FALLING)
    {
        EXTI->RTSR &= ~(getGpioPinMask(io.PinNb));
        EXTI->FTSR |= (getGpioPinMask(io.PinNb));
    }
    else if (io.Trg == IOD::ITRG::RIS_FALL)
    {
        EXTI->RTSR |= getGpioPinMask(io.PinNb);
        EXTI->FTSR |= getGpioPinMask(io.PinNb);
    }
    else
    {
        return false;
    }
    return true;
}




/**
 * @brief Perypheral GPIO Clock Enable
 */
static constexpr bool clockEnable(const GPIO_TypeDef* port)
{
    if (port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else if (port == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
    else if (port == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
    else if (port == GPIOF)
    {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
    else if (port == GPIOG)
    {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
    else if (port == GPIOH)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();
    }
    else if (port == GPIOI)
    {
        __HAL_RCC_GPIOI_CLK_ENABLE();
    }
    else
    {
        return false;
    }

    return true;
}
