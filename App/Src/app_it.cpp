#include <cstdint>
#include <stdio.h>

#include "patterns.hpp"
#include "app.hpp"
#include "app_it.hpp"
#include "api_gpio.hpp"
#include "console.hpp"

/**
 * @brief Global Object Instance of SubjectWithDebouce
 */
SubjectWithDebouce exti0_Subject{200u};

/**
 * @brief Class Debouncer
 */
Debouncer::Debouncer(uint32_t debounceMs) : debounceTime(debounceMs), lastTick(0), locked(false) {}

bool Debouncer::shouldTrigger()
{
    uint32_t now = HAL_GetTick();

    if (now - lastTick > debounceTime)
    {
        lastTick = now;
        locked   = false;
    }

    if (!locked)
    {
        locked = true;
        return true;
    }

    return false;
}

/**
 * @brief  Heatbeat Led Toggle every 500[ms]
 */
extern "C" void HeartBeat_SysTick(void)
{
    static uint32_t ticks = 0u;

    auto ledHeartBeat = ioDispatcher.get("LED_GREEN");

    if (ticks++ >= 500)
    {
        ticks = 0;
        ledHeartBeat.toggle();
    }
}

/**
 * @brief Callback function for Externall Interrupt on Gpio
 */
extern "C" void EXTI0_Callback(uint16_t GPIO_Pin)
{
    exti0_Subject.notifyObserversWhenStable(GPIO_Pin);
}

/**
 * @brief Callback function for Externall Interrupt on Gpio
 */
extern "C" void USART2_Callback(uint32_t t_byte)
{
    ConsolObserver<uint8_t, 1>::instance().notifyObservers(static_cast<uint8_t>(t_byte));
}
