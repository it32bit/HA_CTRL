/**
 ******************************************************************************
 * @file        : app_main.cpp
 * @author      : i32bit
 * @brief       : Entry point and high-level control logic for the application
 ******************************************************************************
 * @copyright   : MIT License
 *                This software is provided "as is", without warranty of any kind.
 *                The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#include <cstring>
#include <stdio.h>
#include "app.hpp"
#include "clock_manager_stm32.hpp"
#include "watchdog_manager_stm32.hpp"


#include "api_debug.hpp"
#include "console.hpp"

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

/**
 * @brief Static functions
 */
static void AppIntro();
static void ClockErrorHandler();

/**
 * @brief Global Objects
 */
ClockManager    clock;
GpioManager     gpio;
WatchdogManager watchdog;
AdcManager      adc;
Console         console;

// float temp = adc.readTemperature();
// printf("Temperature: %.2f°C\n", temp);

/**
 * @brief Main Application entry point for C++ code
 */
extern "C" int main(void)
{
    clock.initialize(ClockErrorHandler);

    HAL_Init();

    /** Initialization code for C++ application can be added here */
    watchdog.initialize(1000); // 1 second timeout
    gpio.initialize(gpioPinConfigs);
    adc.initialize();

    debugInit();

    UserButtonManager usrButton(exti0_Subject, GPIO_PIN_0);
    LedManager        usrLed(exti0_Subject, GPIO_PIN_0, gpio.getPin(PinId::LD_BLU));

    AppIntro();

    __enable_irq();

    /** Main loop */
    for (;;)
    {
        usrButton.process();
        usrLed.process();

        watchdog.feed();
    }
}

extern "C" void WatchdogFeed(void)
{
    watchdog.feed();
}

void ConsoleNotify(uint8_t t_item)
{
    console.receivedData(t_item);
}

/**
 * @brief Application Intro on wake-up
 */
static void AppIntro()
{
    printf("HA-CTRL-APP\t Firmware Version: %d.%d \n\r", FIRMWARE_VERSION.major,
           FIRMWARE_VERSION.minor);
}

/**
 * @brief  This function is executed in case of clock error occurrence.
 */
static void ClockErrorHandler()
{
    uint32_t loop = 0;
    __disable_irq();
    while (1)
    {
        ++loop;
    }
}

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

void UserButtonManager::process()
{
    if (m_pending)
    {
        m_pending = false;
        float temp = adc.readTemperature();
        printf("[%s:%d]:%3d:Temperature: %3.2f[*C]\n\r", getFilename(), __LINE__, static_cast<int>(++m_press_counter), temp);
    }
}

LedManager::LedManager(Subject& t_subject, uint32_t t_ledMask, IGPIOPin* t_led)
    : m_subject(t_subject), m_pin_mask(t_ledMask), m_ledPtr(t_led)
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
        m_ledPtr->toggle();
    }
}

/**
 * @brief Stub implementation of _getentropy for systems without entropy support.
 *
 * This weak symbol always returns -1 to indicate failure, allowing it to be overridden
 * by platform-specific implementations that provide secure randomness.
 */
extern "C" int _getentropy(void* buffer, size_t length) __attribute__((weak));

int _getentropy(void* buffer, size_t length)
{
    return -1; // Always fail, as expected
}
