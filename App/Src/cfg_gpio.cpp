/**
 ******************************************************************************
 * @file        : cfg_gpio.hpp
 * @author      : i32bit
 * @brief       : GPIO configuration and label mapping
 *                for board-level initialization.
 ******************************************************************************
 * This header provides compile-time arrays for GPIO pin setup and name-to-index mapping.
 * The configuration array `ioPinConfigDefArray` defines hardware parameters for each pin,
 * while `pinLabelDefArray` enables access by labels.
 *
 * Usage:
 * @code
 * GpioManager<PIN_CONFIG_ARRAY_SIZE> gpioManager(ioPinConfigDefArray, pinLabelDefArray);
 * gpioManager.get("LED_GREEN").toggle();  // Example usage
 * @endcode
 *
 * These arrays are stored in flash and used directly without dynamic allocation.
 */
#include "api_gpio.hpp"

/**
 * @brief   GPIO PIN configuration array
 *
 * @details This will be located in flash, and the configure function will consume the data in pace,
 *          without the need to populate structs in stack memory
 */
const std::array<IOD, PIN_CONFIG_ARRAY_SIZE> ioPinConfigDefArray = {{
    // green LED 4
    {GPIOD, 12, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},
    // orange LED 3
    {GPIOD, 13, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},
    // red LED 5
    {GPIOD, 14, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},
    // blue LED 6
    {GPIOD, 15, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW},
    // B1 - user button
    {GPIOA, 0, IOD::MODER::INPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::PULLDOWN,
     IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::DONT_CARE},
}};

/**
 * @brief Compile-time pin name to their corresponding index in ioPinConfigDefArray.
 *
 * This pair allows name-based access to GPIO configurations. Each string key (e.g. "LED_GREEN")
 * corresponds to an index in the ioPinConfigDefArray, which holds the actual pin setup.
 *
 * Example:
 * @code
 *      size_t index = boardPinNames.at("LED_RED");         // returns 2
 *      const IOD& pin = ioPinConfigDefArray[index];   // access GPIO definition
 *
 *      // In one line configuration to all pins.
 *      GpioManager<4> gpioMannager(ioPinConfigDefArray, pinLabelDefArray);
 * @endcode
 */
constexpr std::array<std::pair<std::string_view, size_t>, PIN_CONFIG_ARRAY_SIZE> pinLabelDefArray =
    {{{"LED_GREEN", 0}, {"LED_ORANGE", 1}, {"LED_RED", 2}, {"LED_BLUE", 3}, {"BUTTON", 4}}};
