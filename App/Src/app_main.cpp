/**
 ******************************************************************************
 * @file           : app_main.cpp
 * @author         : i32bit
 * @brief          : High-level application control logic
 ******************************************************************************
 * This software is licensed under the MIT License.
 * Provided "as is", without warranty of any kind.
 * The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#include "api_gpio.hpp"
#include "app_main.hpp"

/**
 * @brief   GPIO PIN configuration array
 *
 * @details This will be located in flash, and the configure function will consume the data in pace,
 *          without the need to populate structs in stack memory
 */
static const std::array<IOD, 4> ioDefinitions = {{
    // green LED 4
    {GPIOD, 12, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},
    // orange LED 3
    {GPIOD, 13, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_HIGH},
    // orange LED 5
    {GPIOD, 14, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_HIGH},
    // orange LED 6
    {GPIOD, 15, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_HIGH},

    // ... and so on
}};

const std::map<std::string, size_t> boardPinNames = {
    {"LED4", 0}, {"LED3", 1}, {"LED5", 2}, {"LED6", 3}

    // ... and so on
};

GpioManager<4> gpio(ioDefinitions, boardPinNames);

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
    auto            led4  = gpio.get("LED4");

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
    hal_ConfigGpio(ioDefinitions);

    /** Any initialization code can be added here */
}
