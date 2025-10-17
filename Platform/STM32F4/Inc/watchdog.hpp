#ifndef _WATCHDOG_HPP
#define _WATCHDOG_HPP

#include <stdint-gcc.h>

class Watchdog
{
  public:
    Watchdog(uint32_t t_timeout_ms);
    void feed();

  private:
    void waitReady();

    uint32_t m_reload_value;
};

#endif
