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
#include "clock_manager_stm32.hpp"
#include "gpio_manager_stm32.hpp"
#include "watchdog_manager_stm32.hpp"
#include "api_debug.hpp"
#include "hal_adc.hpp"
#include "console.hpp"

#include <cstring>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

void Error_App_Handler();

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
 * @brief Static declaration
 */
static void AppIntro();

/**
 * @brief Global Objects
 */
Console console;

GpioManager     gpioManager;
WatchdogManager watchdog;
ClockManager    clockManager;

extern "C" void WatchdogFeed(void)
{
    watchdog.feed();
}

/**
 * Main application entry point for C++ code
 */
extern "C" int main(void)
{
    clockManager.initialize(Error_App_Handler);

    HAL_Init();

    /** Initialization code for C++ application can be added here */
    watchdog.initialize(1000); // 1 second timeout
    gpioManager.initialize(gpioPinConfigs);

    ADC_Internal_Init();

    debugInit();

    UserButtonManager usrButton(exti0_Subject, GPIO_PIN_0);
    LedManager        usrLed(exti0_Subject, GPIO_PIN_0, gpioManager.getPin(PinId::LD_BLU));

    AppIntro();

    // Enable global interrupts
    __enable_irq();

    /** App Main loop */
    for (;;)
    {
        usrButton.process();
        usrLed.process();

        watchdog.feed();
    }
}

static void AppIntro()
{
    printf("HA-CTRL-APP\t Firmware Version: %d.%d \n\r", FIRMWARE_VERSION.major,
           FIRMWARE_VERSION.minor);
}

void consoleNotify(uint8_t t_item)
{
    console.receivedData(t_item);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_App_Handler()
{
    uint32_t loop = 0;
    __disable_irq();
    while (1)
    {
        ++loop;
    }
}

extern "C" int _getentropy(void* buffer, size_t length) __attribute__((weak));

int _getentropy(void* buffer, size_t length)
{
    return -1; // Always fail, as expected
}