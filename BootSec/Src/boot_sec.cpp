/**
 * @file      BootSec/Src/boot_sec.cpp
 * @author    it32bit
 * @brief     Secendary bootloader stage for Platform (STM32F4, ESP32, etc.).
 *            Initializes system clock, manages flash-backed BootState flags,
 *            and jumps to the application (App).
 *
 * @version   1.0
 * @date      2025-10-24
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include <cstdint>
#include "boot.hpp"
#include "stm32f4xx_hal.h"
#include "boot_flag_manager.hpp"
#include "flash_writer_stm32.hpp"
#include "flash_layout.hpp"
#include "clock_manager_stm32.hpp"
#include "gpio_manager_stm32.hpp"

// Access Metadata and Cert Regions
// const auto* metadata     = reinterpret_cast<const Firmware::Metadata*>(FlashLayout::METADATA_START);
// const auto* cert_pub     = reinterpret_cast<const uint8_t*>(FlashLayout::CERT_PUBLIC_START);
// const auto* cert_priv    = reinterpret_cast<const uint8_t*>(FlashLayout::CERT_PRIVATE_START);
// const auto* error_log    = reinterpret_cast<const uint8_t*>(FlashLayout::ERROR_LOG_START);

// int main()
// {
//     FlashWriterSTM32F4 flash;
//     Bootloader         boot(&flash);

//     if (boot.isMetadataValid() && boot.verifyStagedFirmware())
//     {
//         boot.applyUpdate();
//     }

//     boot.jumpToApplication();
// }

static void DeinitPeripheralsBeforeJump();
static void JumpToApp();
static void ClockErrorHandler();

ClockManager clock;
GpioManager  gpio;

extern "C" int main()
{
    FlashWriterSTM32F4 writer;
    BootFlagManager    flags(&writer);

    clock.initialize(ClockErrorHandler);
    gpio.initialize(gpioPinConfigs);

    if (auto red = gpio.getPin(PinId::LD_RED))
    {
        red->toggle();
    }

    /**
    * TODO: Firmware verification support should be added once the process of writing the new firmware to flash memory is completed.
    *
    if (flags.getState() == BootState::Staged)
    {
        // TODO: Add CRC or signature verification here
        flags.setState(BootState::Verified);
    }

    if (flags.getState() == BootState::Verified)
    {
        // TODO: Apply update (e.g., copy NEW_APP to APP)
        flags.setState(BootState::Applied);
    }
    */
    JumpToApp();

    while (true)
    {
    }
}

static void JumpToApp()
{
    constexpr std::uintptr_t addr      = FlashLayout::APP_START;
    uint32_t                 stack_ptr = *reinterpret_cast<volatile uint32_t*>(addr);
    uint32_t                 reset_ptr = *reinterpret_cast<volatile uint32_t*>(addr + 4);

    __disable_irq();
    DeinitPeripheralsBeforeJump();
    SCB->VTOR = addr;
    __DSB();
    __ISB();
    __set_MSP(stack_ptr);

    auto entry = reinterpret_cast<void (*)()>(reset_ptr);
    entry();

    while (true)
    {
    }
}

static void DeinitPeripheralsBeforeJump()
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

static void ClockErrorHandler()
{
    __disable_irq();
    while (true)
    {
    }
}
