/**
 * @file      Platform/Interface/PilUart/uart_id_stm32.hpp
 * @author    it32bit
 * @brief     Defines UartId enum for compile-time selection of UART peripherals.
 *            Enables type-safe configuration of USART instances across STM32 targets.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef UART_ID_HPP
#define UART_ID_HPP

#include <stdint-gcc.h>

enum class UartId : uint8_t
{
    Uart1 = 0,
    Uart2,
    Uart3,
    Uart6
};

#endif
