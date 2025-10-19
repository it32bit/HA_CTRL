/**
 * @file      Platform/STM32F4/Inc/uart_redirect.hpp
 * @author    it32bit
 * @brief     Declares redirect hook for printf() output via UartManager.
 *            Enables integration of standard I/O with ha-ctrl UART abstraction.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef UART_REDIRECT_HPP
#define UART_REDIRECT_HPP

class UartManager;
void setUartRedirect(UartManager& manager);

#endif
