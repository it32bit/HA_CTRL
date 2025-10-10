#include "stm32f4xx_ll_iwdg.h"
#include <stdint-gcc.h>
#include "watchdog.hpp"

Watchdog::Watchdog(uint32_t t_timeout_ms)
{
    // Prescaler = 64, Tick = 64 / 32_000 = 2 ms
    constexpr uint32_t prescaler = 64;

    const uint32_t lsi_freq     = 32000;
    const uint32_t tick_time_ms = (prescaler * 1000) / lsi_freq;

    m_reload_value = t_timeout_ms / tick_time_ms;
    if (m_reload_value > 0x0FFF)
    {
        m_reload_value = 0x0FFF;
    }

    // Start the watchdog
    LL_IWDG_Enable(IWDG);

    // Enable write access
    LL_IWDG_EnableWriteAccess(IWDG);

    // Set prescaler to 64
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_64);

    // Set reload value
    LL_IWDG_SetReloadCounter(IWDG, m_reload_value);

    waitReady();

    feed();
}

void Watchdog::feed()
{
    LL_IWDG_ReloadCounter(IWDG);
}

void Watchdog::waitReady()
{
    while (LL_IWDG_IsReady(IWDG) == 0)
    {
        // Wait until the registers are updated (SR bits cleared)
    }
}
