#include <cstring>
#include "console.hpp"
#include "stm32f4xx.h"

void Console::receivedData(uint8_t t_item)
{
    if (t_item == '\r' || t_item == '\n')
    {
        m_buffer[m_pos] = '\0';
        processLine(m_buffer.data());
        m_buffer[0] = '\0';
        m_pos       = 0;
    }
    else if (m_pos < m_max_length - 1)
    {
        m_buffer[m_pos++] = t_item;
    }
    else
    {
        m_buffer[0] = '\0';
        m_pos       = 0;
    }
}

void Console::processLine(const char* line)
{
    if (strcmp(line, "help") == 0)
    {
        send("Available commands: help, echo, reset\r\n");
    }
    else if (strncmp(line, "echo ", 5) == 0)
    {
        send(line + 5);
        send("\r\n");
    }
    else if (strcmp(line, "reset") == 0)
    {
        NVIC_SystemReset();
    }
    else
    {
        if (m_buffer[0] != '\0') // Prevention, double termination \n\r
            send("Unknown command\r\n");
    }
}

void Console::send(const char* msg)
{
    while (*msg)
    {
        while (!(USART2->SR & USART_SR_TXE))
            ;
        USART2->DR = *msg++;
    }
}