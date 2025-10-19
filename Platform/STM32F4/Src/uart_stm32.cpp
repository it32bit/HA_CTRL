/**
 * @file      Platform/STM32F4/Src/uart_stm32.cpp
 * @author    it32bit
 * @brief     Implements UART driver for STM32F4 using LL drivers.
 *            Supports basic TX/RX operations for console communication.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include "uart_stm32.hpp"
#include "stm32f4xx_ll_rcc.h"

static IRQn_Type resolveIrq(UartId id);

static IRQn_Type resolveIrq(UartId id)
{
    switch (id)
    {
        case UartId::Uart1:
            return USART1_IRQn;
        case UartId::Uart2:
            return USART2_IRQn;
        case UartId::Uart3:
            return USART3_IRQn;
        case UartId::Uart6:
            return USART6_IRQn;
        default:
            return NonMaskableInt_IRQn;
    }
}

void Uart_STM32::init(UartId id, uint32_t baudrate)
{
    switch (id)
    {
        case UartId::Uart1:
            __HAL_RCC_USART1_CLK_ENABLE();
            m_usart = USART1;
            break;
        case UartId::Uart2:
            __HAL_RCC_USART2_CLK_ENABLE();
            m_usart = USART2;
            break;
        case UartId::Uart3:
            __HAL_RCC_USART3_CLK_ENABLE();
            m_usart = USART3;
            break;
        case UartId::Uart6:
            __HAL_RCC_USART6_CLK_ENABLE();
            m_usart = USART6;
            break;
    }

    uint32_t brr = getAPBClockFreq(id) / baudrate;

    m_usart->CR1 = 0;
    m_usart->BRR = brr;
    m_usart->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
    m_usart->CR1 |= USART_CR1_RXNEIE; // Enable RX interrupt
    NVIC_EnableIRQ(resolveIrq(id));      
}

void Uart_STM32::write(char c)
{
    while (!(m_usart->SR & USART_SR_TXE))
    {
    }
    m_usart->DR = c;
}

void Uart_STM32::write(const char* str)
{
    while (*str)
    {
        write(*str++);
    }
}

uint32_t Uart_STM32::getAPBClockFreq(UartId id)
{
    uint32_t sysclk = SystemCoreClock;
    uint32_t presc  = 0;

    if (id == UartId::Uart1 || id == UartId::Uart6)
    {
        presc = APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE2) >> RCC_CFGR_PPRE2_Pos];
    }
    else
    {
        presc = APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos];
    }

    return sysclk >> presc;
}

bool Uart_STM32::read(char& out)
{
    if (m_usart && (m_usart->SR & USART_SR_RXNE))
    {
        out = static_cast<char>(m_usart->DR);
        return true;
    }
    return false;
}
