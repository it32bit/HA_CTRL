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
#include <array>
#include "pin_config.hpp"

using enum PinConfig::Mode;
using enum PinConfig::Pull;
using enum PinConfig::Type;
using enum PinConfig::Speed;
using enum PinConfig::InterruptTrigger;
using enum PinConfig::PortIndex;

/**
 * @brief   GPIO PIN configuration array
 *
 * @details This will be located in flash, and the configure function will consume the data in pace,
 *          without the need to populate structs in stack memory
 */

constexpr std::size_t PIN_CONFIG_ARRAY_SIZE = 7; // Adjust as needed

// TODO: add to PinConfig the field similar to IOD::ISTATE to define initial state of output pins
constexpr std::array<PinConfig, PIN_CONFIG_ARRAY_SIZE> gpioPinConfigs = {
    {{"BUTTON", PortA, 0, Input, PullDown, PushPull, Low, Rising, 0, 15},
     {"UART2_TX", PortA, 2, Alternate, PullNone, PushPull, VeryHigh, None, 7, 15},
     {"UART2_RX", PortA, 3, Alternate, PullNone, PushPull, VeryHigh, None, 7, 15},
     {"LED_GREEN", PortD, 12, Output, PullNone, PushPull, Low, None, 0, 15},
     {"LED_ORANGE", PortD, 13, Output, PullNone, PushPull, Low, None, 0, 15},
     {"LED_RED", PortD, 14, Output, PullNone, PushPull, Low, None, 0, 15},
     {"LED_BLUE", PortD, 15, Output, PullNone, PushPull, Low, None, 0, 15}}
};
/** (*Note 1) Alternate function mapping number for STM32: USART2 on PA2/PA3 is GPIO_AF7_USART2 = 7 */

// Static assert to ensure the array size matches the defined size
// static_assert(sizeof(gpioPinConfigs) / sizeof(gpioPinConfigs[0]) == PIN_CONFIG_ARRAY_SIZE, "GPIO config array size mismatch");

// TODO: to be removed, replaced by std::array above
// const std::array<IOD, PIN_CONFIG_ARRAY_SIZE> ioPinConfigDefArray = {
//     {
//      // green LED 4
//         {GPIOD, 12, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
//          IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW, 15u},
//      // orange LED 3
//         {GPIOD, 13, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
//          IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW, 15u},
//      // red LED 5
//         {GPIOD, 14, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
//          IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW, 15u},
//      // blue LED 6
//         {GPIOD, 15, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
//          IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW, 15u},
//      // B1 - user button
//         {GPIOA, 0, IOD::MODER::INPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::PULLDOWN,
//          IOD::GEXTI::IT, IOD::ITRG::RISING, IOD::ISTATE::DONT_CARE, 15u},
//      // UART2-TX
//         {GPIOA, 2, IOD::MODER::ALT, GPIO_AF7_USART2, IOD::TYPE::NORMAL, IOD::SPEED::VERYHIGH,
//          IOD::PUPDR::NO, IOD::GEXTI::IT, IOD::ITRG::RISING, IOD::ISTATE::DONT_CARE, 15u},
//      // UART2-RX
//         {GPIOA, 3, IOD::MODER::ALT, GPIO_AF7_USART2, IOD::TYPE::NORMAL, IOD::SPEED::VERYHIGH,
//          IOD::PUPDR::NO, IOD::GEXTI::IT, IOD::ITRG::RISING, IOD::ISTATE::DONT_CARE, 15u},
//      }
// };
