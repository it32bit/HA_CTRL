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
#include "hal_adc.hpp"
#include "console.hpp"
#include "watchdog.hpp"
#include <cstring>
#include <stdio.h>
#include "stm32f4xx_hal.h"

/**
 * @brief Lambda getFilename
 */
auto getFilename = []() -> const char*
{
    const char* path     = __FILE__;
    const char* filename = std::strrchr(const_cast<char*>(path), '/');
    const char* ret      = (filename != nullptr) ? filename + 1 : path;
    return ret;
};

UserButtonManager::UserButtonManager(Subject& t_subject, uint32_t t_pin_mask)
    : m_subject(t_subject), m_pin_mask(t_pin_mask)
{
    m_subject.registerObserver(this);
}

void UserButtonManager::notify(uint32_t t_pin_mask) const
{
    if ((t_pin_mask & m_pin_mask) != 0)
    {
        m_pending = true;
    }
}

void UserButtonManager::process()
{
    if (m_pending)
    {
        m_pending = false;
        printf("[%s:%d]:%d\n\r", getFilename(), __LINE__, static_cast<int>(++m_press_counter));
    }
}

LedManager::LedManager(Subject& t_subject, uint32_t t_ledMask, const PinController& t_led)
    : m_subject(t_subject), m_pin_mask(t_ledMask), m_led(t_led)
{
    m_subject.registerObserver(this);
}

void LedManager::notify(uint32_t mask) const
{
    if ((mask & m_pin_mask) != 0)
    {
        m_pending = true;
    }
}

void LedManager::process()
{
    if (m_pending)
    {
        m_pending = false;
        m_led.toggle();
    }
}

/**
 * @brief Static declaration
 *          AppInit_cpp
 */
static void AppInit();
static void AppIntroduction();

/**
 * @brief Global Objects
 *          Console console
 */
Console console;

Watchdog watchdog(100);

extern "C" void WatchdogFeed(void)
{
    watchdog.feed();
}

/**
 * Main application entry point for C++ code
 */
extern "C" int main(void)
{
    HAL_Init();
    /** Initialization code for C++ application can be added here */
    AppInit();

    WatchdogFeed();

    UserButtonManager usrButton(exti0_Subject, GPIO_PIN_0);
    LedManager        usrLed(exti0_Subject, GPIO_PIN_0, ioDispatcher.get("LED_BLUE"));

    AppIntroduction();

    /** App Main loop */
    for (;;)
    {
        usrButton.process();
        usrLed.process();

        WatchdogFeed();
    }
}

/**
 * Initialization function for C++ application
 */
static void AppInit()
{
    gpioConfig(ioPinConfigDefArray);
    ADC_Internal_Init();

    debugInit();

    /** Any initialization code can be added here */
}

static void AppIntroduction()
{
    printf("HA-CTRL:\n\r Firmware Version: %d.%d \n\r", FIRMWARE_VERSION.major,
           FIRMWARE_VERSION.minor);
}

void consoleNotify(uint8_t t_item)
{
    console.receivedData(t_item);
}
