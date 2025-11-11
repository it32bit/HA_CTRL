/**
 * @file      Platform/STM32F4/Inc/uart_manager_stm32.hpp
 * @author    it32bit
 * @brief     Declares UartManager for static instantiation and access to the UART driver.
 *            Provides safe initialization and console I/O delegation without dynamic allocation.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef UART_MANAGER_STM32_HPP
#define UART_MANAGER_STM32_HPP

#include <cstddef>
#include "pil_uart.hpp"
#include "uart_stm32.hpp"

class UartManager
{
  public:
    UartManager();
    void initialize(UartId id, uint32_t baudrate);
    void write(char c);
    void write(const char* str);

    IConsoleUart* getUart();

  private:
    static constexpr std::size_t MaxSize  = sizeof(Uart_STM32);
    static constexpr std::size_t MaxAlign = alignof(Uart_STM32);
    alignas(MaxAlign) std::byte m_storage[MaxSize];
    IConsoleUart* m_uart = nullptr;
};

#endif
