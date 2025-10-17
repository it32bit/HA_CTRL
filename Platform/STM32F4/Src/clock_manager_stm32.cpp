#include "clock_manager_stm32.hpp"
#include "clock_stm32.hpp"
#include <new>

ClockManager::ClockManager() = default;

void ClockManager::initialize(void (*handler)())
{
    m_clock = new (m_storage) Clock_STM32F4();
    m_clock->configure(handler);
}
