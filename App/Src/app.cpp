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
#include <cstring>

auto getFilename = []() -> const char*
{
    const char* path     = __FILE__;
    const char* filename = std::strrchr(path, '/');
    return filename ? filename + 1 : path;
};

UserButtonManager::UserButtonManager(Subject& subject)
    : subject_(subject)
{
    subject_.registerObserver(this);
}

void UserButtonManager::notify() const
{
    pending_ = true;
}

void UserButtonManager::process()
{
    if (pending_)
    {
        pending_ = false;
        printf("[%s:%d]\n\r", getFilename(), __LINE__);
    }
}

LedManager::LedManager(Subject& subject, const PinController& led)
    : subject_(subject), led_(led)
{
    subject_.registerObserver(this);
}

void LedManager::notify() const
{
    pending_ = true;
}

void LedManager::process()
{
    if (pending_)
    {
        pending_ = false;
        led_.toggle();
    }
}

/**
 * @brief Static declaration
 *          AppInit_cpp
 */
static void AppInit_cpp();

SubjectWithDebouce exti0_Subject{200u};

/**
 * Main application entry point for C++ code
 */
extern "C" void App_cpp(void)
{
    /** Initialization code for C++ application can be added here */
    AppInit_cpp();

    UserButtonManager usrButton(exti0_Subject);
    LedManager        ledManager(exti0_Subject, ioDispatcher.get("LED_BLUE"));

    printf("HA-CTRL:\n\r Firmware Version: %d.%d \n\r", FIRMWARE_VERSION.major,
           FIRMWARE_VERSION.minor);

    /** App Main loop */
    for (;;)
    {
        usrButton.process();
        ledManager.process();
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
