/**
 * @file      BootPrim/Src/boot_prim.cpp
 * @author    it32bit
 * @brief     Primary bootloader stage for Platform (STM32F4, ESP32, etc.).
 *            Initializes system clock, manages flash-backed BootState flags,
 *            and jumps to the secondary bootloader (BootSec).
 *
 * @version   1.0
 * @date      2025-10-23
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include <cstdint>
#include "boot_prim.hpp"
#include "boot_flag_manager.hpp"
#include "stm32f4xx.h"
#include "clock_manager_stm32.hpp"
#include "flash_writer_stm32.hpp"
#include "flash_layout.hpp"
#include "firmware_metadata.hpp"
#include "crc32_check.hpp"
#include "image_manager.hpp"

namespace BootPrim
{

static void DeinitPeripheralsBeforeJumpPrime()
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

static void JumpToBootSec()
{
    constexpr std::uintptr_t addr      = FlashLayout::BOOTLOADER2_START;
    uint32_t                 stack_ptr = *reinterpret_cast<volatile uint32_t*>(addr);
    uint32_t                 reset_ptr = *reinterpret_cast<volatile uint32_t*>(addr + 4);

    __disable_irq();
    DeinitPeripheralsBeforeJumpPrime();

    SCB->VTOR = addr;
    __DSB();
    __ISB();
    __set_MSP(stack_ptr);

    auto entry = reinterpret_cast<void (*)()>(reset_ptr);
    entry();
}

ClockManager clock;

extern "C" int main()
{
    clock.initialize(nullptr);

    FlashWriterSTM32F4 writer;
    BootFlagManager    flags(&writer);
    ImageManager       image(&writer);

    /**
     * Boot-Sec
     */
    bool newBootSecCandidateCheck = isImageAuthentic(FlashLayout::NEW_BOOTLOADER2_START,
                                            FlashLayout::NEW_BOOTLOADER2_METADATA_START);

    bool newBootSecCandidateDiffrent = isImageDiffrent(FlashLayout::BOOT2_METADATA_START,
                                                FlashLayout::NEW_BOOTLOADER2_METADATA_START);
    if (newBootSecCandidateCheck == true)
    {
        if (newBootSecCandidateDiffrent == true)
        {
            image.writeImage(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::BOOTLOADER2_START,
                             FlashLayout::NEW_BOOTLOADER2_SIZE);

            LEDControl::toggleBlueLED();
        }

        image.clearImage(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::NEW_BOOTLOADER2_SIZE);
    }

    /**
     * Application
     */
    bool newAppCheck = isImageStaged(FlashLayout::NEW_APP_METADATA_START);

    auto isStaged = flags.getState();

    if (newAppCheck == true)
    {
        if (isStaged != BootState::Staged)
        {
            flags.setState(BootState::Staged);
        }

        LEDControl::toggleOrangeLED();
    }

    /**
     * Secend Bootloader Integrity check
     */
    bool bootSecCheck =
        isImageAuthentic(FlashLayout::BOOTLOADER2_START, FlashLayout::BOOT2_METADATA_START);

    if (bootSecCheck == true)
    {
        JumpToBootSec();
    }

    /**
     * Secend Bootloader Integrity check FAIL
     */
    uint32_t timer{};

    while (true)
    {
        /**
         * @brief Blinking period depends on number of uC cycles per loop (~30 - 50):
         *      Tc = 1/16MHz = 62.5ns,
         *      Tloop = Tc x CyclePerLoop,
         *      Ttoggle = Tloop x 1M,
         *  Obserwed blinking period ~1Hz
         */
        if ((timer % 1000000) == 0)
        {
            LEDControl::toggleOrangeLED();
        }
        ++timer;
    }
}

} // namespace BootPrim
