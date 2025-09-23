/**
 ******************************************************************************
 * @file           : ha_ctrl.cpp
 * @author         : i32bit
 * @brief          : High-level application control logic
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 */
#include "led_ctrl.hpp"
#include "mod_hal_gpio.hpp"
#include "ha_ctrl.hpp"

/**
 * @brief GPIO configuration
 * @details This will be located in flash, and the configure function will consume the data in pace,
 * without the need to populate structs in stack memory
 */
static const std::array<GPIODEF, 4> gpiodefs = {{
    // green LED 4
    {GPIOD, 12, GPIODEF::IOFUNCTION::OUTPUT, 0, GPIODEF::IOTYPE::NORMAL, GPIODEF::IOSPEED::LOW,
     GPIODEF::IOPULL::NONEPULL, GPIODEF::IOEXTI::NONEEXTI, GPIODEF::IOTRIGGER::NONETRG,
     GPIODEF::IOSTATE::LOGIC_LOW},
    // orange LED 3
    {GPIOD, 13, GPIODEF::IOFUNCTION::OUTPUT, 0, GPIODEF::IOTYPE::NORMAL, GPIODEF::IOSPEED::LOW,
     GPIODEF::IOPULL::NONEPULL, GPIODEF::IOEXTI::NONEEXTI, GPIODEF::IOTRIGGER::NONETRG,
     GPIODEF::IOSTATE::LOGIC_LOW},
    // orange LED 5
    {GPIOD, 14, GPIODEF::IOFUNCTION::OUTPUT, 0, GPIODEF::IOTYPE::NORMAL, GPIODEF::IOSPEED::LOW,
     GPIODEF::IOPULL::NONEPULL, GPIODEF::IOEXTI::NONEEXTI, GPIODEF::IOTRIGGER::NONETRG,
     GPIODEF::IOSTATE::LOGIC_LOW},
    // orange LED 6
    {GPIOD, 15, GPIODEF::IOFUNCTION::OUTPUT, 0, GPIODEF::IOTYPE::NORMAL, GPIODEF::IOSPEED::LOW,
     GPIODEF::IOPULL::NONEPULL, GPIODEF::IOEXTI::NONEEXTI, GPIODEF::IOTRIGGER::NONETRG,
     GPIODEF::IOSTATE::LOGIC_LOW},

    // ... and so on
}};

LedController led3(GPIOD, GPIO_PIN_13);

LedController led5(GPIOD, GPIO_PIN_14);
LedController led6(GPIOD, GPIO_PIN_15);

static void AppInit_cpp();

/**
 * Main application entry point for C++ code
 */
extern "C" void App_cpp(void)
{
    /** Initialization code for C++ application can be added here */
    AppInit_cpp();

    /** Main loop */
    for (;;)
    {
    }
}

/**
 * @brief  Heatbeat LDx Led Toggle
 */
extern "C" void HeartBeat_SysTick(void)
{
    static uint32_t ticks = 0;
    LedController led4(GPIOD, GPIO_PIN_12);

    if (ticks++ >= 500) // Toggle every 500ms
    {
        ticks = 0;
        led4.toggle();
    }
}

/**
 * Initialization function for C++ application
 */
static void AppInit_cpp()
{
    Configure(gpiodefs);

    led3.on();
    led5.on();
    led6.on();

    /** Any initialization code can be added here */
}
