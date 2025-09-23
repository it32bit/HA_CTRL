#include "mod_hal_gpio.hpp"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"

static constexpr bool     gpio_ClockEnable(const GPIO_TypeDef* port);
static constexpr uint32_t gpio_ExtiLine(const uint32_t PinNumber);
static constexpr uint32_t gpio_PinPositionTable(const uint32_t PinNumber);

static constexpr uint32_t gpio_ExtiLine(const uint32_t PinNumber)
{
    return (PinNumber & 0x03U) | ((PinNumber - 1u) << 16U);
}

constexpr uint32_t gpio_PinPositionTable(const uint32_t PinNumber)
{
    if (PinNumber > 15u)
        return 0u;
    return (uint32_t)1u << (PinNumber);
}

/**
 * @brief Configure the port pins
 */
bool Configure(const GPIODEF& def)
{
    gpio_ClockEnable(def.GPIO);

    LL_GPIO_SetPinSpeed(def.GPIO, gpio_PinPositionTable(def.PinNumber), def.Speed);
    LL_GPIO_SetPinOutputType(def.GPIO, gpio_PinPositionTable(def.PinNumber), def.Type);
    LL_GPIO_SetPinPull(def.GPIO, gpio_PinPositionTable(def.PinNumber), def.Bias);

    if (def.Function == GPIODEF::IOFUNCTION::ANALOG)
    {
        if (def.PinNumber < 9u)
        {
            LL_GPIO_SetAFPin_0_7(def.GPIO, gpio_PinPositionTable(def.PinNumber), def.AltFunc);
        }
        else
        {
            LL_GPIO_SetAFPin_8_15(def.GPIO, gpio_PinPositionTable(def.PinNumber), def.AltFunc);
        }
    }
    LL_GPIO_SetPinMode(def.GPIO, gpio_PinPositionTable(def.PinNumber), def.Function);

    /* Configure the External Interrupt or event for the current IO */
    if (def.Exti != GPIODEF::IOEXTI::NONEEXTI)
    {
        /* Enable SYSCFG Clock */
        __HAL_RCC_SYSCFG_CLK_ENABLE();

        LL_SYSCFG_SetEXTISource(GPIO_GET_INDEX(def.GPIO), gpio_ExtiLine(def.PinNumber));

        /* Clear Rising Falling edge configuration */
        LL_SYSCFG_SetEXTISource(GPIO_GET_INDEX(def.GPIO), gpio_ExtiLine(def.PinNumber));

        // Configure EXTI line
        gpio_PinPositionTable(def.PinNumber);

        LL_EXTI_EnableIT_0_31(gpio_PinPositionTable(def.PinNumber));

        if (def.Trigg == GPIODEF::IOTRIGGER::RISING)
        {
            LL_EXTI_EnableRisingTrig_0_31(gpio_PinPositionTable(def.PinNumber));
        }
        else
        {
            LL_EXTI_EnableFallingTrig_0_31(gpio_PinPositionTable(def.PinNumber));
        }

        LL_EXTI_EnableEvent_0_31(gpio_ExtiLine(def.PinNumber));

        /* Clear EXTI line configuration */
        LL_EXTI_EnableIT_0_31(gpio_ExtiLine(def.PinNumber));
    }

    return true;
}

/**
 * @brief Perypheral GPIO Clock Enable
 */
static constexpr bool gpio_ClockEnable(const GPIO_TypeDef* port)
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
