/**
 ******************************************************************************
 * @file        : app_main.cpp
 * @author      : i32bit
 * @brief       : High-level application control logic
 ******************************************************************************
 * This software is licensed under the MIT License.
 * Provided "as is", without warranty of any kind.
 * The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#include "api_gpio.hpp"
#include "app_main.hpp"

extern GpioDispatcher<PIN_CONFIG_ARRAY_SIZE> ioDispatcher;

/**
 * @brief Static declaration
 *          AppInit_cpp
 */
static void AppInit_cpp();
static void UserButton_Handler();

static uint32_t b1 = 0u;

/**
 * Main application entry point for C++ code
 */
extern "C" void App_cpp(void)
{
    /** Initialization code for C++ application can be added here */
    AppInit_cpp();

    auto button = ioDispatcher.get("BUTTON");

    /** Main loop */
    for (;;)
    {
        b1 = button.readInputPin();
    }
}

/**
 * Initialization function for C++ application
 */
static void AppInit_cpp()
{
    hal_ConfigGpio(ioPinConfigDefArray);

    /** Any initialization code can be added here */
}

static void UserButton_Handler()
{
    auto ledButton = ioDispatcher.get("LED_BLUE");
    ledButton.toggle();
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
