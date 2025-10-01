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
#include "console.hpp"
#include <cstring>
#include <functional>

/**
 * @brief Lambda getFilename
 */
auto getFilename = []() -> const char*
{
    const char* path     = __FILE__;
    const char* filename = std::strrchr(path, '/');
    return filename ? filename + 1 : path;
};

UserButtonManager::UserButtonManager(Subject& t_subject, uint32_t t_pin_mask)
    : m_subject(t_subject), m_pin_mask(t_pin_mask)
{
    m_subject.registerObserver(this);
}

void UserButtonManager::notify(uint32_t t_pin_mask) const
{
    if (t_pin_mask & m_pin_mask)
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

LedManager::LedManager(Subject& subject, uint32_t ledMask, const PinController& led)
    : m_subject(subject), m_pin_mask(ledMask), m_led(led)
{
    m_subject.registerObserver(this);
}

void LedManager::notify(uint32_t mask) const
{
    if (mask & m_pin_mask)
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
 * Main application entry point for C++ code
 */
extern "C" void App(void)
{
    /** Initialization code for C++ application can be added here */
    AppInit();

    UserButtonManager usrButton(exti0_Subject, GPIO_PIN_0);
    LedManager        usrLed(exti0_Subject, GPIO_PIN_0, ioDispatcher.get("LED_BLUE"));

    Console console;
    auto    consoleCallback = std::bind(&Console::receivedData, &console, std::placeholders::_1);
    ConsolObserver<uint8_t, 1>::instance().registerObserver(consoleCallback);

    AppIntroduction();

    /** App Main loop */
    for (;;)
    {
        usrButton.process();
        usrLed.process();
    }
}

/**
 * Initialization function for C++ application
 */
static void AppInit()
{
    gpioConfig(ioPinConfigDefArray);

    debugInit();

    /** Any initialization code can be added here */
}

static void AppIntroduction()
{
    printf("HA-CTRL:\n\r Firmware Version: %d.%d \n\r", FIRMWARE_VERSION.major,
           FIRMWARE_VERSION.minor);
}
