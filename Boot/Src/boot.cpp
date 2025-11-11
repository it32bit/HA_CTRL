/**
 * @file      boot.cpp
 * @author    it32bit
 * @brief     Bootloader jump logic
 *
 * @version   1.0
 * @date      2025-10-24
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */

#include "boot.hpp"
#include "stm32f4xx.h"

void Bootloader::jumpToAddress(const std::uintptr_t t_jump_address)
{
    uint32_t stack_ptr = *reinterpret_cast<volatile uint32_t*>(t_jump_address);
    uint32_t reset_ptr = *reinterpret_cast<volatile uint32_t*>(t_jump_address + 4);

    __disable_irq();
    deinitPeripherals();

    SCB->VTOR = t_jump_address;
    __DSB();
    __ISB();
    __set_MSP(stack_ptr);

    auto entry = reinterpret_cast<void (*)()>(reset_ptr);
    entry();

    while (true)
    {
    }
}

void Bootloader::deinitPeripherals()
{
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    for (uint32_t i = 0; i < sizeof(NVIC->ICER) / sizeof(NVIC->ICER[0]); ++i)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    RCC->CR |= RCC_CR_HSION;
    RCC->CFGR = 0;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0)
    {
    }

    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);
    while ((RCC->CR & RCC_CR_PLLRDY) != 0)
    {
    }

    RCC->PLLCFGR = 0x24003010;
    RCC->CIR     = 0;

    RCC->AHB1ENR = 0;
    RCC->AHB2ENR = 0;
    RCC->AHB3ENR = 0;
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;

    EXTI->IMR  = 0;
    EXTI->EMR  = 0;
    EXTI->RTSR = 0;
    EXTI->FTSR = 0;
    EXTI->PR   = 0xFFFFFFFF;

    constexpr GPIO_TypeDef* ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH};
    for (auto port : ports)
    {
        if (!port)
            continue;
        port->MODER   = 0xFFFFFFFF;
        port->OTYPER  = 0;
        port->OSPEEDR = 0;
        port->PUPDR   = 0;
        port->ODR     = 0;
    }

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    SYSCFG->MEMRMP = 0;
}
