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
const std::array<IOD, PIN_CONFIG_ARRAY_SIZE> ioPinConfigDefArray = {
    {
     // green LED 4
        {GPIOD, 12, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
         IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW, 15u},
     // orange LED 3
        {GPIOD, 13, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
         IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW, 15u},
     // red LED 5
        {GPIOD, 14, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
         IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW, 15u},
     // blue LED 6
        {GPIOD, 15, IOD::MODER::OUTPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::NO,
         IOD::GEXTI::NONE, IOD::ITRG::NOTRG, IOD::ISTATE::LOGIC_LOW, 15u},
     // B1 - user button
        {GPIOA, 0, IOD::MODER::INPUT, 0, IOD::TYPE::NORMAL, IOD::SPEED::LOW, IOD::PUPDR::PULLDOWN,
         IOD::GEXTI::IT, IOD::ITRG::RISING, IOD::ISTATE::DONT_CARE, 15u},
     // UART2-TX
        {GPIOA, 2, IOD::MODER::ALT, GPIO_AF7_USART2, IOD::TYPE::NORMAL, IOD::SPEED::VERYHIGH,
         IOD::PUPDR::NO, IOD::GEXTI::IT, IOD::ITRG::RISING, IOD::ISTATE::DONT_CARE, 15u},
     // UART2-RX
        {GPIOA, 3, IOD::MODER::ALT, GPIO_AF7_USART2, IOD::TYPE::NORMAL, IOD::SPEED::VERYHIGH,
         IOD::PUPDR::NO, IOD::GEXTI::IT, IOD::ITRG::RISING, IOD::ISTATE::DONT_CARE, 15u},
     }
};


