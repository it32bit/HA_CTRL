#ifndef CLOCK_MANAGER_HPP
#define CLOCK_MANAGER_HPP

#include <cstddef>
#include <cstdint>
#include <cstddef>
#include <cstdint>
#include "pil_clock_config.hpp"

class ClockManager
{
  public:
    ClockManager();

    void initialize(void (*handler)());

  private:
    static constexpr std::size_t MaxClockSize  = sizeof(uint32_t) * 8;
    static constexpr std::size_t MaxClockAlign = alignof(uint32_t);

    alignas(MaxClockAlign) std::byte m_storage[MaxClockSize];
    IClockConfigurator* m_clock = nullptr;
};

#endif // CLOCK_MANAGER_HPP
