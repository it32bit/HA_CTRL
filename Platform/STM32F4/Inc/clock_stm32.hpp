#ifndef CLOCK_STM32_HPP
#define CLOCK_STM32_HPP

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
#include "pil_clock_config.hpp"

class Clock_STM32F4 : public IClockConfigurator
{
  public:
    void configure(void (*handler)()) override;

  private:
    void configureSystemClock();
    void (*m_errorHandler)() = nullptr; // Optional error handler
};

#endif // CLOCK_STM32_HPP
