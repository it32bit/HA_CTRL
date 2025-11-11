/**
 * @file      Platform/Interface/PilUart/pil_uart.hpp
 * @author    it32bit
 * @brief     Declares the IConsoleUart interface for platform-independent UART communication.
 *            Enables portable console output and input handling across MCU targets.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef PIL_UART_HPP
#define PIL_UART_HPP

#include "uart_id_stm32.hpp"

class IConsoleUart
{
  public:
    virtual void init(UartId id, uint32_t baudrate) = 0;
    virtual void write(char c)                      = 0;
    virtual bool read(char& out)                    = 0;
    virtual ~IConsoleUart()                         = default;
};

#endif
