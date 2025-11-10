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
#include "boot_sec.hpp"
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

static void ClockErrorHandler();

static void ClockErrorHandler()
{
    __disable_irq();
    while (true)
    {
    }
}

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
    auto red    = gpio.getPin(PinId::LD_RED);
    auto orange = gpio.getPin(PinId::LD_ORA);

    red->set();

    if (Shared::firmwareUpdateFlag == Shared::PREPARE_TO_RECEIVE_BINARY)
    {
        Shared::firmwareUpdateFlag = 0;
        orange->set();

        receiver.receiveImage(FlashLayout::NEW_BOOTLOADER2_START,
                              FlashLayout::NEW_BOOTLOADER2_SIZE + FlashLayout::NEW_APP_TOTAL_SIZE);
        orange->reset();

        candidateReceived = true;
    }

    /**
     * If flags == BootState::Staged then: App Image is compared with metadata
     */
    if ((flags.getState() == BootState::Staged) || (candidateReceived == true))
    {
        bool newAppCandidateDiffrent = isImageDiffrent(FlashLayout::APPLICATION_METADATA_START,
                                                       FlashLayout::NEW_APP_METADATA_START);

        if (newAppCandidateDiffrent == true)
        {
            bool newAppCandidateCheck =
                isImageAuthentic(FlashLayout::NEW_APP_START, FlashLayout::NEW_APP_METADATA_START);

            if (newAppCandidateCheck == true)
            {
                // At this point, you can set the "Verified" flag, then check it in the application (e.g., via CLI).
                // After verification, set the "Applied" flag — boot-sec will detect it, move the new image to the application area,
                // and then jump to the application.
                // But at this point, the old application is replaced with the new one here.
                image.writeImage(FlashLayout::NEW_APP_START, FlashLayout::APP_START,
                                 FlashLayout::NEW_APP_TOTAL_SIZE);
                flags.setState(BootState::Applied);
            }
            else
            {
                flags.setState(BootState::Failed);
            }
        }
        image.clearImage(FlashLayout::NEW_APP_START, FlashLayout::NEW_APP_TOTAL_SIZE);
    }
    else
    {
        bool newAppCandidateFirmwareEmpty =
            isImageEmpty(FlashLayout::NEW_APP_START, FlashLayout::NEW_APP_TOTAL_SIZE);
        bool newAppCandidateMetaEmpty =
            isImageEmpty(FlashLayout::NEW_APP_METADATA_START, FlashLayout::NEW_APP_METADATA_SIZE);
        if ((newAppCandidateFirmwareEmpty == false) || (newAppCandidateMetaEmpty == false))
        {
            image.clearImage(FlashLayout::NEW_APP_START, FlashLayout::NEW_APP_TOTAL_SIZE);
        }
    }

    bool appCheck =
        isImageAuthentic(FlashLayout::APP_START, FlashLayout::APPLICATION_METADATA_START);

    if (appCheck == true)
    {
        red->reset();
        Bootloader::jumpToAddress(FlashLayout::APP_START);
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
        if ((timer % 1000000) == 0)
        {
            red->toggle();
        }
        ++timer;
    }
}
