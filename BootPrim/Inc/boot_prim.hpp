// boot_prim.hpp
#include "stm32f4xx.h"

class LEDControl
{
  public:
    static void enableOrangeLED()
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

        // 6. Turn on LED (set PD13 high)
        GPIOD->ODR |= (1 << 13);
    }

    static void enableBlueLED()
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

        // 6. Turn on LED (set PD15 high)
        GPIOD->ODR |= (1 << 15);
    }
};
