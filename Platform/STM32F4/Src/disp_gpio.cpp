/**
 ******************************************************************************
 * @file        : disp_gpio.hpp
 * @author      : i32bit
 * @brief       : Dispatcher for GPIO perypheral
 ******************************************************************************
 * This software is licensed under the MIT License.
 * Provided "as is", without warranty of any kind.
 * The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#include "api_gpio.hpp"

/**
 * @brief Initializes the GPIO manager with pin configuration and label definitions.
 *
 * Creates an instance of GpioManager with a fixed array size defined by PIN_CONFIG_ARRAY_SIZE.
 * The manager is responsible for handling GPIO initialization, mapping pin labels to hardware pins,
 * and providing access to configured GPIO functionality.
 *
 * @param ioPinConfigDefArray Array containing hardware-specific GPIO configuration structures.
 * @param pinLabelDefArray Array mapping human-readable pin labels to their corresponding indices.
 */
GpioDispatcher<PIN_CONFIG_ARRAY_SIZE> ioDispatcher(ioPinConfigDefArray, pinLabelDefArray);
