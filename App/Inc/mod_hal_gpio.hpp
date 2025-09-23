#ifndef MOD_HAL_GPIO_HPP
#define MOD_HAL_GPIO_HPP

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <array>
#include <ranges>
#include "stm32f4xx.h"


using GPIODEF = struct IODefStuct
{
    GPIO_TypeDef* GPIO;

    uint32_t PinNumber;

    enum IOFUNCTION : uint32_t
    {
        INPUT    = 0,
        OUTPUT   = 1,
        ALT      = 2,
        ANALOG   = 3,
        OUT_TYPE = 16
    } Function;

    uint32_t AltFunc;

    enum IOTYPE : uint32_t
    {
        NORMAL    = 0,
        OPENDRAIN = 1
    } Type;

    enum IOSPEED : uint32_t
    {
        LOW      = 0,
        MEDIUM   = 1,
        HIGH     = 2,
        VERYHIGH = 3
    } Speed;

    enum IOPULL : uint32_t
    {
        NONEPULL = 0,
        PULLUP   = 1,
        PULLDOWN = 2
    } Bias;

    enum IOEXTI : uint32_t
    {
        NONEEXTI = 0,
        IT       = 1,
        EVT      = 2
    } Exti;

    enum IOTRIGGER : uint32_t
    {
        NONETRG = 0,
        RISING  = 1,
        FALLING = 2
    } Trigg;

    enum IOSTATE : uint32_t
    {
        LOGIC_LOW,
        LOGIC_HIGH,
        DONT_CARE
    } InitialState;
};

bool Configure(const GPIODEF& def);

template <typename Iter> bool Configure(Iter begin, Iter end)
{
    while (begin != end)
    {
        Configure(*begin++);
    }
    return true;
}

template <typename T>
    requires std::ranges::range<T>
bool Configure(const T& definitions)
{
    for (auto& def : definitions)
    {
        Configure(def);
    }
    return true;
}

#endif // MOD_HAL_GPIO_HPP
