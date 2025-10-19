/**
 * @file      Platform/STM32F4/Src/uart_manager_stm32.cpp
 * @author    it32bit
 * @brief     Implements UartManager for STM32F4 platform.
 *            Constructs Uart_STM32 in-place and delegates console I/O via IConsoleUart interface.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include "uart_manager_stm32.hpp"
#include "uart_stm32.hpp"
#include <new>

UartManager::UartManager() = default;

void UartManager::initialize(UartId id, uint32_t baudrate)
{
    m_uart = new (m_storage) Uart_STM32();
    m_uart->init(id, baudrate);
}

void UartManager::write(char c)
{
    if (m_uart)
    {
        m_uart->write(c);
    }
}

void UartManager::write(const char* str)
{
    if (!m_uart)
    {
        return;
    }
    while (*str)
    {
        m_uart->write(*str++);
    }
}
