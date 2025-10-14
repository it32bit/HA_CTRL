/**
 ******************************************************************************
 * @file        : gpio_config_stm32.hpp
 * @author      : i32bit
 * @brief       : GPIO configuration and label mapping header
 *                for board-level initialization.
 ******************************************************************************
 * This header provides compile-time arrays for GPIO pin setup and name-to-index mapping.
 * The configuration array `gpioPinConfigs` defines hardware parameters and labels for each pin,
 * enabling platform-agnostic initialization code to configure GPIOs based on this data.
 *
 * Usage for Application and Bootloader:
 * @code
 *  #include "cfg_gpio.hpp"
 *  #include "gpio_manager_stm32.hpp"
 *
 *  constexpr std::array<std::string_view, 2> pinLabels = {"LED1", "BUTTON1"};
 *  GpioManager gpioManager;
 *  gpioManager.initializePins(pinConfigs);
 *  IGPIOPin* ledPin = gpioManager.getPin("LED1");
 *  ledPin->set();
 * @endcode
 *
 * These arrays are stored in flash and used directly without dynamic allocation.
 */
#include <array>
#include "pil_pin_config.hpp"

using enum PinConfig::Mode;
using enum PinConfig::Pull;
using enum PinConfig::Type;
using enum PinConfig::Speed;
using enum PinConfig::InterruptTrigger;
using enum PinConfig::PortIndex;
using enum PinConfig::InterruptExti;

constexpr std::size_t PIN_CONFIG_ARRAY_SIZE = 7; // Adjust as needed

/**
 * @brief   GPIO PIN configuration array
 *
 * @details This will be located in flash, and the configure function will consume the data in pace,
 *          without the need to populate structs in stack memory
 */

// TODO: add to PinConfig the field similar to IOD::ISTATE to define initial state of output pins
constexpr std::array<PinConfig, PIN_CONFIG_ARRAY_SIZE> gpioPinConfigs = {
    {{"BUTTON", PortA, 0, Input, PullDown, PushPull, Low, ExtiIT, Rising, 0, 15},
     {"CLI_TX", PortA, 2, Alternate, PullNone, PushPull, VeryHigh, ExtiNone, None, 7, 15},
     {"CLI_RX", PortA, 3, Alternate, PullNone, PushPull, VeryHigh, ExtiNone, None, 7, 15},
     {"LD4_GR", PortD, 12, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {"LD3_OR", PortD, 13, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {"LD5_RE", PortD, 14, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {"LD6_BL", PortD, 15, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15}}
};

/** (*Note 1) Alternate function mapping number for STM32: USART2 on PA2/PA3 is GPIO_AF7_USART2 = 7 */
/** (*Note 2) CLI = Command Line Interface, used for serial console over UART */

// Static assert to ensure the array size matches the defined size
// static_assert(sizeof(gpioPinConfigs) / sizeof(gpioPinConfigs[0]) == PIN_CONFIG_ARRAY_SIZE, "GPIO config array size mismatch");
