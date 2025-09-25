#include "hal_gpio.hpp"
#include "stm32f4xx_ll_gpio.h"


static constexpr bool clockEnable(const GPIO_TypeDef* port);

/**
 * @section Inline hal_gpio
 */
using gpioAction = void (*)(GPIO_TypeDef*, uint32_t);

inline void setOutputLow(GPIO_TypeDef* gpio, uint32_t pin)
{
    LL_GPIO_ResetOutputPin(gpio, pin);
}

inline void setOutputHigh(GPIO_TypeDef* gpio, uint32_t pin)
{
    LL_GPIO_SetOutputPin(gpio, pin);
}

inline void setOutputNoop(GPIO_TypeDef*, uint32_t) {}

/** IOD -> enum ISTATE : uint32_t {DONT_CARE,     LOGIC_LOW,    LOGIC_HIGH} */
inline std::array<gpioAction, 3> outputDispatcher = {setOutputNoop, setOutputLow, setOutputHigh};

/**
 * @brief Lower-level hal_ConfigGpio(def) function to handle individual GPIO setup.
 */
bool hal_ConfigGpio(const IOD& io)
{
    clockEnable(io.GPIO);

    LL_GPIO_SetPinSpeed(io.GPIO, getGpioPinMask(io.PinNb), io.Speed);
    LL_GPIO_SetPinOutputType(io.GPIO, getGpioPinMask(io.PinNb), io.Type);
    LL_GPIO_SetPinPull(io.GPIO, getGpioPinMask(io.PinNb), io.Pupdr);

    if (io.Moder == IOD::MODER::OUTPUT)
    {
        /** Replace switch-case for IOD::MODER::OUTPUT */
        outputDispatcher[static_cast<std::size_t>(io.InitState)](io.GPIO, getGpioPinMask(io.PinNb));
    }
    else if (io.Moder == IOD::MODER::ANALOG)
    {
        if (io.PinNb < 9u)
        {
            LL_GPIO_SetAFPin_0_7(io.GPIO, getGpioPinMask(io.PinNb), io.AltFunc);
        }
        else
        {
            LL_GPIO_SetAFPin_8_15(io.GPIO, getGpioPinMask(io.PinNb), io.AltFunc);
        }
    }

    LL_GPIO_SetPinMode(io.GPIO, getGpioPinMask(io.PinNb), io.Moder);

    /**
     * @deprecated Configure the External Interrupt or event for the current IO
     *             NOT SUPPORTET
     */
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
