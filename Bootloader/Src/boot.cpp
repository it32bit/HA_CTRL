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

#include "gpio_manager_stm32.hpp"

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
static void DeinitPeripheralsBeforeJump();
static void JumpToApp();
static void Error_Boot_Handler();

static GpioManager gpio;

extern "C" int main(void)
{
    SystemClock_Config();

    gpio.initialize(gpioPinConfigs);

    auto red = gpio.getPin(PinId::LD_RED);

    if (red)
    {
        red->toggle();
    }

    JumpToApp();

    while (1)
    {
    };
}

// extern "C" uint32_t APP_ADDRESS;

static void JumpToApp()
{
    constexpr uint32_t APP_ADDRESS       = 0x08020000;
    uint32_t           app_stack         = *(volatile uint32_t*)(APP_ADDRESS);
    uint32_t           app_reset_handler = *(volatile uint32_t*)(APP_ADDRESS + 4);

    // Disable interrupts
    __disable_irq();

    // De-initialize peripherals to their reset state
    DeinitPeripheralsBeforeJump();

    // Set vector table offset
    SCB->VTOR = APP_ADDRESS;

    // Ensure all memory accesses are completed
    __DSB(); // Ensure all memory accesses are completed
    __ISB(); // Flush and refill pipeline with updated permissions

    // Set main stack pointer
    __set_MSP(app_stack);

    // Function pointer to app reset handler
    void (*app_entry)(void) = (void (*)(void))app_reset_handler;

    app_entry();

    // Should never return here
    for (;;)
    {
    };
}

static void DeinitPeripheralsBeforeJump()
{
    // 1. Disable SysTick
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    // 2. Clear NVIC (interrupt controller)
    constexpr uint32_t irq_count = sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]);
    for (uint32_t i = 0; i < irq_count; ++i)
    {
        NVIC->ICER[i] = 0xFFFFFFFF; // Disable all interrupts
        NVIC->ICPR[i] = 0xFFFFFFFF; // Clear all pending IRQs
    }

    // 3. Reset RCC to default state
    RCC->CR |= RCC_CR_HSION; // Enable HSI
    RCC->CFGR = 0x00000000;  // Reset clock configuration
    while ((RCC->CR & RCC_CR_HSIRDY) == 0)
        ; // Wait for HSI ready

    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON); // Disable HSE, PLL
    while ((RCC->CR & RCC_CR_PLLRDY) != 0)
        ; // Wait for PLL to stop

    RCC->PLLCFGR = 0x24003010; // Reset PLL config
    RCC->CIR     = 0x00000000; // Disable all clock interrupts

    // 4. Disable all peripheral clocks (AHB1, AHB2, APB1, APB2)
    RCC->AHB1ENR = 0x00000000;
    RCC->AHB2ENR = 0x00000000;
    RCC->AHB3ENR = 0x00000000;
    RCC->APB1ENR = 0x00000000;
    RCC->APB2ENR = 0x00000000;

    // 5. Clear all pending EXTI lines
    EXTI->IMR  = 0x00000000;
    EXTI->EMR  = 0x00000000;
    EXTI->RTSR = 0x00000000;
    EXTI->FTSR = 0x00000000;
    EXTI->PR   = 0xFFFFFFFF; // Clear all EXTI flags

    // 6. Reset GPIOs to analog mode to save power and avoid floating inputs
    constexpr GPIO_TypeDef* GPIO_PORTS[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH};
    for (auto port : GPIO_PORTS)
    {
        if (port == nullptr)
            continue;
        port->MODER   = 0xFFFFFFFF; // Analog mode
        port->OTYPER  = 0x00000000;
        port->OSPEEDR = 0x00000000;
        port->PUPDR   = 0x00000000;
        port->ODR     = 0x00000000;
    }

    // Optional: Reset SYSCFG and PWR
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    SYSCFG->MEMRMP = 0;
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