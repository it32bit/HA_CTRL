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
 * @brief Compile-time pin name to their corresponding index in ioPinConfiguratnionArray.
 *
 * This pair allows name-based access to GPIO configurations. Each string key (e.g. "LED_GREEN")
 * corresponds to an index in the ioPinConfiguratnionArray, which holds the actual pin setup.
 *
 * Example:
 * @code
 *      size_t index = boardPinNames.at("LED_RED");         // returns 2
 *      const IOD& pin = ioPinConfiguratnionArray[index];   // access GPIO definition
 *
 *      // In one line configuration to all pins.
 *      GpioManager<4> gpioMannager(ioPinConfiguratnionArray, pinLabelDefArray);
 * @endcode
 */
constexpr std::array<std::pair<std::string_view, size_t>, 4> pinLabelDefArray = {
    {{"LED_GREEN", 0}, {"LED_ORANGE", 1}, {"LED_RED", 2}, {"LED_BLUE", 3}}};

GpioManager<4> gpioMannager(ioPinConfiguratnionArray, pinLabelDefArray);

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
