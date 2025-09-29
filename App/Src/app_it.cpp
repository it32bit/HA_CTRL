#include <cstdint>
#include <stdio.h>
#include "app_it.hpp"
#include "api_gpio.hpp"
#include "hal_gpio.hpp"

static void UserButton_Handler();

class Debouncer
{
  public:
    Debouncer(uint32_t debounceMs) : debounceTime(debounceMs), lastTick(0), locked(false) {}

    bool shouldTrigger()
    {
        uint32_t now = HAL_GetTick();

        if (now - lastTick > debounceTime)
        {
            lastTick = now;
            locked   = false;
        }

        if (locked == false)
        {
            locked = true;
            return true;
        }

        return false;
    }

  private:
    uint32_t debounceTime;
    uint32_t lastTick;
    bool     locked;
};

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
 * @brief  Callback function for Externall Interrupt on Gpio
 */
extern "C" void EXTI0_Callback(uint16_t GPIO_Pin)
{
    static Debouncer debounceUserButton(200); // 200 ms debounce

    if (GPIO_Pin == GPIO_PIN_0)
    {
        if (debounceUserButton.shouldTrigger())
        {
            UserButton_Handler();
        }
    }
}

static void UserButton_Handler()
{
    auto ledButton = ioDispatcher.get("LED_BLUE");
    ledButton.toggle();

    printf("[%s:%d]\n\r", __FILE_NAME__, __LINE__);
}
