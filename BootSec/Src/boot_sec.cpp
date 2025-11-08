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
#include "uart_receiver_stm32.hpp"
#include "uart_manager_stm32.hpp"
#include "image_manager.hpp"
#include "shared_memory.hpp"

// Access Metadata and Cert Regions
// const auto* metadata     = reinterpret_cast<const Firmware::Metadata*>(FlashLayout::METADATA_START);
// const auto* cert_pub     = reinterpret_cast<const uint8_t*>(FlashLayout::CERT_PUBLIC_START);
// const auto* cert_priv    = reinterpret_cast<const uint8_t*>(FlashLayout::CERT_PRIVATE_START);
// const auto* error_log    = reinterpret_cast<const uint8_t*>(FlashLayout::ERROR_LOG_START);

static void DeinitPeripheralsBeforeJump();
static void JumpToApp();
static void ClockErrorHandler();

ClockManager clock;
GpioManager  gpio;
UartManager  uart;

extern "C" int main()
{
    FlashWriterSTM32F4 writer;
    BootFlagManager    flags(&writer);
    ImageManager       image(&writer);

    clock.initialize(ClockErrorHandler);
    gpio.initialize(gpioPinConfigs);
    uart.initialize(UartId::Uart2, 115200);
    UartReceiver receiver(*uart.getUart(), writer);

    bool candidateReceived{false};

    if (auto red = gpio.getPin(PinId::LD_RED))
    {
        red->set();
    }

    if (Shared::firmwareUpdateFlag == Shared::PREPARE_TO_RECEIVE_BINARY)
    {
        receiver.receiveImage(FlashLayout::NEW_BOOTLOADER2_START,
                              FlashLayout::NEW_BOOTLOADER2_SIZE + FlashLayout::NEW_APP_TOTAL_SIZE);
        Shared::firmwareUpdateFlag = 0;
        candidateReceived          = true;
    }

    /**
     * If flags == BootState::Staged then: App Image is compared with metadata
     */
    if ((flags.getState() == BootState::Staged) || (candidateReceived == true))
    {
        bool newAppCandidateCheck =
            isImageAuthentic(FlashLayout::NEW_APP_START, FlashLayout::NEW_APP_METADATA_START);

        bool newAppCandidateDiffrent = isImageDiffrent(FlashLayout::APPLICATION_METADATA_START,
                                                       FlashLayout::NEW_APP_METADATA_START);

        if (newAppCandidateCheck == true)
        {
            if (newAppCandidateDiffrent == true)
            {
                // At this point, you can set the "Verified" flag, then check it in the application (e.g., via CLI).
                // After verification, set the "Applied" flag — boot-sec will detect it, move the new image to the application area,
                // and then jump to the application.
                // But at this point, the old application is replaced with the new one here.
                image.writeImage(FlashLayout::NEW_APP_START, FlashLayout::APP_START,
                                 FlashLayout::NEW_APP_TOTAL_SIZE);
                flags.setState(BootState::Applied);
            }
        }
        else
        {
            flags.setState(BootState::Failed);
            if (auto orange = gpio.getPin(PinId::LD_ORA))
            {
                orange->reset();
            }
        }
        image.clearImage(FlashLayout::NEW_APP_START, FlashLayout::NEW_APP_TOTAL_SIZE);
    }

    bool appCheck =
        isImageAuthentic(FlashLayout::APP_START, FlashLayout::APPLICATION_METADATA_START);

    if (appCheck == true)
    {
        JumpToApp();
    }

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
        auto red = gpio.getPin(PinId::LD_RED);

        if ((timer % 1000000) == 0)
        {
            if (red != nullptr)
            {
                red->toggle();
            }
        }
        ++timer;
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
