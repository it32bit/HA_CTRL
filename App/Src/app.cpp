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

#include "app.hpp"
#include "api_gpio.hpp"
#include "api_debug.hpp"

/**
 * @brief Static declaration
 *          AppInit_cpp
 */
static void AppInit_cpp();

/**
 * Main application entry point for C++ code
 */
extern "C" void App_cpp(void)
{
    /** Initialization code for C++ application can be added here */
    AppInit_cpp();

    printf("HA-CTRL:\n\r Firmware Version: %d.%d \n\r", FIRMWARE_VERSION.major,
           FIRMWARE_VERSION.minor);

    /** App Main loop */
    for (;;)
    {
    }
}

/**
 * Initialization function for C++ application
 */
static void AppInit_cpp()
{
    hal_ConfigGpio(ioPinConfigDefArray);
    serialDebuger_Init();

    /** Any initialization code can be added here */
}
