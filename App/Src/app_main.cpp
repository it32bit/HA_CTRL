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

/**
 * @brief   GPIO PIN configuration array
 *
 * @details This will be located in flash, and the configure function will consume the data in pace,
 *          without the need to populate structs in stack memory
 */
static const std::array<IOD, 4> ioPinConfiguratnionArray = {{
    // green LED 4
    {GPIOD, 12, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},
    // orange LED 3
    {GPIOD, 13, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},
    // orange LED 5
    {GPIOD, 14, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},
    // orange LED 6
    {GPIOD, 15, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},

    // ... and so on
}};

/**
 * @brief Maps board pin names to their corresponding index in ioPinConfiguratnionArray.
 *
 * This map allows name-based access to GPIO configurations. Each string key (e.g. "LD4")
 * corresponds to an index in the ioPinConfiguratnionArray, which holds the actual pin setup.
 *
 * Example:
 * @code
 *      size_t index = boardPinNames.at("LED_RED");         // returns 2
 *      const IOD& pin = ioPinConfiguratnionArray[index];   // access GPIO definition
 *      // In one line to all pins (size )
 *      GpioManager<4> gpioMannager(ioPinConfiguratnionArray, pinNamesMap);
 * @endcode
 */
const std::map<std::string, size_t> pinNamesMap = {
    {"LED_GREEN", 0}, {"LED_ORANGE", 1}, {"LED_RED", 2}, {"LED_BLUE", 3}};

GpioManager<4> gpioMannager(ioPinConfiguratnionArray, pinNamesMap);

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
 * @brief  Heatbeat Led Toggle every 500[ms]
 */
extern "C" void HeartBeat_SysTick(void)
{
    static uint32_t ticks = 0u;

    auto ledHeartBeat = gpioMannager.get("LED_GREEN");

    if (ticks++ >= 500)
    {
        ticks = 0;
        ledHeartBeat.toggle();
    }
}

/**
 * Initialization function for C++ application
 */
static void AppInit_cpp()
{
    hal_ConfigGpio(ioPinConfiguratnionArray);

    /** Any initialization code can be added here */
}

/**
 * @warning WARN-10 Provide a stub to silence the warning:
 *          libc_a-getentropyr.o: _getentropy is not implemented and will always fail
 */
extern "C" int _getentropy(void* buffer, size_t length)
{
    return -1; // It will override the weak symbol from libc.a
}