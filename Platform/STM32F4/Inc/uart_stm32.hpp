/**
 * @file      Platform/STM32F4/Inc/uart_stm32.hpp
 * @author    it32bit
 * @brief     Provides STM32F4-specific implementation of the IConsoleUart interface.
 *            Wraps low-level UART configuration and interrupt-driven I/O using LL drivers.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef UART_STM32_HPP
#define UART_STM32_HPP

#include "pil_uart.hpp"
#include "stm32f4xx.h"




class Uart_STM32 : public IConsoleUart
{
  public:
    void init(UartId id, uint32_t baudrate) override;
    bool read(char& out) override;
    void write(char c) override;

    void write(const char* str);

  private:
    USART_TypeDef* m_usart = nullptr;
    uint32_t       getAPBClockFreq(UartId id);
};

#endif
