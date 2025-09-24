#include "mod_hal_gpio.hpp"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"

static constexpr bool     clockEnable(const GPIO_TypeDef* port);
static constexpr uint32_t extiLine(const uint32_t PinNumber);
static constexpr uint32_t pinPosition(const uint32_t PinNumber);
static constexpr uint32_t extiLine(const uint32_t PinNumber);

/**
 * @brief Lower-level hal_ConfigGpio(def) function to handle individual GPIO setup.
 */
bool hal_ConfigGpio(const IOD& io)
{
    clockEnable(io.GPIO);

    LL_GPIO_SetPinSpeed(io.GPIO, pinPosition(io.PinNumber), io.Speed);
    LL_GPIO_SetPinOutputType(io.GPIO, pinPosition(io.PinNumber), io.Type);
    LL_GPIO_SetPinPull(io.GPIO, pinPosition(io.PinNumber), io.Pupdr);

    if (io.Mode == IOD::MODER::ANALOG)
    {
        if (io.PinNumber < 9u)
        {
            LL_GPIO_SetAFPin_0_7(io.GPIO, pinPosition(io.PinNumber), io.AltFunc);
        }
        else
        {
            LL_GPIO_SetAFPin_8_15(io.GPIO, pinPosition(io.PinNumber), io.AltFunc);
        }
    }
    LL_GPIO_SetPinMode(io.GPIO, pinPosition(io.PinNumber), io.Mode);

    if(io.Mode == IOD::MODER::OUTPUT)
    {
        
    }

/**
 * @deprecated Configure the External Interrupt or event for the current IO
 *             NOT SUPPORTET
 */
    return true;
}

/**
 * @section Static function hal_gpio.cpp
 */

static constexpr uint32_t extiLine(const uint32_t PinNumber)
{
    return (PinNumber & 0x03U) | ((PinNumber - 1u) << 16U);
}

static constexpr uint32_t pinPosition(const uint32_t PinNumber)
{
    if (PinNumber > 15u)
        return 0u;
    return (uint32_t)1u << (PinNumber);
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
