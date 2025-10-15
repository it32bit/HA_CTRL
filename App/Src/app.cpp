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
#include "gpio_manager_stm32.hpp"

#include "api_debug.hpp"
#include "hal_adc.hpp"
#include "console.hpp"
#include "watchdog.hpp"
#include <cstring>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

/**
 * @brief System Clock Configuration
 * @retval None
 * @details
 * The system Clock is configured as follows :
 *    System Clock source            = PLL (HSE)
 *    SYSCLK(Hz)                     = 168000000
 *    HCLK(Hz)                       = 168000000
 *    AHB Prescaler                  = 1
 *    AHB Perypheral(Hz) Memory, DMA = 168000000
 *    APB1 Prescaler                 = 4
 *    APB1 Perypheral(Hz)            = 42000000
 *    APB2 Prescaler                 = 2
 *    APB2 Perypheral(Hz)            = 84000000
 *    HSE Frequency(Hz)              = 8000000
 *    PLL_M                          = 8
 *    PLL_N                          = 336
 *    PLL_P                          = 2
 *    PLL_Q                          = 7
 *    VDD(V)                         = 3.3
 *    Main regulator output voltage  = Scale1 mode
 *    Flash Latency(WS)              = 5
 */
static void SystemClock_Config(void);

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
        // m_ledPtr->toggle();
    }
}

/**
 * @brief Static declaration
 */
static void AppIntro();

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

GpioManager gpioManager;

/**
 * Main application entry point for C++ code
 */
extern "C" int main(void)
{
    SystemClock_Config();
    HAL_Init();

    /** Initialization code for C++ application can be added here */
    gpioManager.initialize(gpioPinConfigs);
    auto userButton = gpioManager.getPin("BUTTON");

    auto led = gpioManager.getPin("LD_BLU");

    if (led)
    {
        led->set();
    }

    ADC_Internal_Init();

    debugInit();
    WatchdogFeed();

    UserButtonManager usrButton(exti0_Subject, GPIO_PIN_0);
    LedManager        usrLed(exti0_Subject, GPIO_PIN_0, gpioManager.getPin("LD_GRE"));

    AppIntro();

    // Enable global interrupts
    __enable_irq();

    /** App Main loop */
    for (;;)
    {
        usrButton.process();
        usrLed.process();

        WatchdogFeed();
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

static void Error_App_Handler(void);

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {};

    /** Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
    in the RCC_OscInitTypeDef structure. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 8;
    RCC_OscInitStruct.PLL.PLLN       = 336;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_App_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_App_Handler();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
static void Error_App_Handler(void)
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