/**
 ******************************************************************************
 * @file           : boot.c
 * @author         : it32bit
 * @brief          : Bootloader: Main program body
 ******************************************************************************
 * This software is licensed under the MIT License.
 * Provided "as is", without warranty of any kind.
 * The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "boot.hpp"

#include "gpio_config_stm32.hpp"

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
static void JumpToApp();
static void Error_Boot_Handler();

constexpr const PinConfig* redCfg = findPinConfig("LD_RED");
GpioPin_STM32*             redLed = GpioPin_STM32::createStatic(*redCfg);


extern "C" int main(void)
{
    SystemClock_Config();

    redLed->set();

    JumpToApp();

    while (1)
    {
    };
}

static void JumpToApp()
{
    uint32_t app_stack         = *(volatile uint32_t*)(APP_ADDRESS);
    uint32_t app_reset_handler = *(volatile uint32_t*)(APP_ADDRESS + 4);

    // Disable interrupts
    __disable_irq();

    // Set vector table offset
    SCB->VTOR = APP_ADDRESS;

    // Ensure all memory accesses are completed
    __DSB();
    __ISB();

    // Set main stack pointer
    __set_MSP(app_stack);

    // Disable SysTick
    // SysTick->CTRL = 0;

    // Function pointer to app reset handler
    void (*app_entry)(void) = (void (*)(void))app_reset_handler;

    app_entry();

    // Should never return here
    for (;;)
    {
    };
}

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
        Error_Boot_Handler();
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
        Error_Boot_Handler();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Boot_Handler(void)
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