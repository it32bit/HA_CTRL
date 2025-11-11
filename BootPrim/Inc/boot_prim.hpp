// boot_prim.hpp
#ifndef BOOT_PRIM_HPP
#define BOOT_PRIM_HPP

#include "stm32f4xx.h"

#include "boot.hpp"
// Define the instance in a specific section (used for linking, bootloader, etc.)
__attribute__((section(".firmware_version"), used)) constexpr FirmwareVersion FIRMWARE_VERSION = {
    .major = 1, .minor = 0};

class LEDControl
{
  public:
    static void toggleOrangeLED()
    {
        // 1. Enable GPIOD clock
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

        // 2. Set PD13 as output
        GPIOD->MODER &= ~(0b11 << (13 * 2)); // Clear mode
        GPIOD->MODER |= (0b01 << (13 * 2));  // Set to output

        // 3. Set output type to push-pull
        GPIOD->OTYPER &= ~(1 << 13);

        // 4. Set high speed (optional)
        GPIOD->OSPEEDR |= (0b11 << (13 * 2));

        // 5. Disable pull-up/pull-down
        GPIOD->PUPDR &= ~(0b11 << (13 * 2));

        // 6. Toggle LED (PD13)
        GPIOD->ODR ^= (1 << 13);
    }

    static void toggleBlueLED()
    {
        // 1. Enable GPIOD clock
        RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

        // 2. Set PD15 as output
        GPIOD->MODER &= ~(0b11 << (15 * 2)); // Clear mode
        GPIOD->MODER |= (0b01 << (15 * 2));  // Set to output

        // 3. Set output type to push-pull
        GPIOD->OTYPER &= ~(1 << 15);

        // 4. Set high speed (optional)
        GPIOD->OSPEEDR |= (0b11 << (15 * 2));

        // 5. Disable pull-up/pull-down
        GPIOD->PUPDR &= ~(0b11 << (15 * 2));

        // 6. Toggle on LED (set PD15 high)
        GPIOD->ODR ^= (1 << 15);
    }
};

#endif // BOOT_PRIM_HPP
