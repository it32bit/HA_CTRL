/**
 ******************************************************************************
 * @file           : boot.c
 * @author         : it32bit
 * @brief          : Main entry point and control logic for the bootloader
 ******************************************************************************
 * @copyright      : MIT License
 *                   This software is provided "as is", without warranty of any kind.
 *                   The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#include <stdint.h>
#include "boot.hpp"
#include "clock_manager_stm32.hpp"
#include "gpio_manager_stm32.hpp"

#include "stm32f4xx_hal.h"

static void DeinitPeripheralsBeforeJump();
static void JumpToApp();
static void ClockErrorHandler();

/**
 * @brief Global Objects
 */
ClockManager* clock = nullptr;
GpioManager*  gpio  = nullptr;

extern "C" int main(void)
{
    // HAL_Init(); // Ensure HAL is ready

    clock = new ClockManager();
    clock->initialize(ClockErrorHandler);

    gpio = new GpioManager();
    gpio->initialize(gpioPinConfigs);

    auto red = gpio->getPin(PinId::LD_RED);

    if (red)
    {
        red->toggle();
    }

    JumpToApp();

    while (1)
    {
    };
}

// constexpr uint32_t APP_ENTRY_ADDRESS = 0x08020000;

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

/**
 * @brief  This function is executed in case of clock error occurrence.
 */
static void ClockErrorHandler(void)
{
    uint32_t loop = 0;
    __disable_irq();
    while (1)
    {
        ++loop;
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

void HardFault_Handler(void)
{
    while (1)
        ; // Optionally blink LED or log fault
}

void UsageFault_Handler(void)
{
    while (1)
        ;
}

void MemManage_Handler(void)
{
    while (1)
        ;
}

void BusFault_Handler(void)
{
    while (1)
        ;
}

void WWDG_IRQHandler(void)
{
    while (1)
        ; // Or clear interrupt flag if WWDG is used
}