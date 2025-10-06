#include "stm32f4xx.h"
#include "hal_adc.hpp"
#include "console.hpp"
#include <cstring>
#include <cstdio>

void Console::receivedData(uint8_t byte) noexcept
{
    if (isBufferFull())
    {
        cleanMessage();
    }

    if (isMessageEnded(byte))
    {
        if (!isBufferEmpty())
        {
            setMessageToProcess();
            process(buffer);
            cleanMessage();
        }
    }
    else
    {
        push(byte);
    }
}

void Console::process(const char* t_line)
{
    for (const auto& cmd : cmdConfigArray)
    {
        if (strncmp(t_line, cmd.name, strlen(cmd.name)) == 0)
        {
            const char* param = t_line + strlen(cmd.name);
            while (*param == ' ')
                ++param;
            cmd.handler(param);
            return;
        }
    }

    send("Unknown command\r\n");
}

void Console::send(const char* t_msg)
{
    while (*t_msg)
    {
        while (!(USART2->SR & USART_SR_TXE))
            ;
        USART2->DR = *t_msg++;
    }
}

bool Console::isBufferFull() const noexcept
{
    return m_head >= MaxLength - 1;
}

bool Console::isBufferEmpty() const noexcept
{
    return m_head == 0;
}

bool Console::isMessageEnded(uint8_t byte) const noexcept
{
    return byte == '\r' || byte == '\n';
}

bool Console::push(uint8_t byte) noexcept
{
    buffer[m_head++] = byte;
    return true;
}

void Console::cleanMessage() noexcept
{
    m_head    = 0;
    buffer[0] = '\0';
}

void Console::setMessageToProcess() noexcept
{
    buffer[m_head] = '\0';
}

void Console::help(const char* t_item)
{
    size_t item_size = std::strlen(t_item);

    for (const auto& cmd : cmdConfigArray)
    {
        if (strncmp(t_item, cmd.name, strlen(cmd.name)) == 0)
        {
            send(cmd.name);
            send(" - ");
            send(cmd.description);
            send("\r\n");
            break;
        }
        else if (!item_size)
        {
            Console::send(cmd.name);
            Console::send(" - ");
            Console::send(cmd.description);
            Console::send("\r\n");
        }
    }
}

void Console::reset(const char* t_item)
{
    NVIC_SystemReset();
}

void Console::echo(const char* t_item)
{
    send(t_item);
    send("\r\n");
}

void Console::temperature(const char* t_item)
{
    float temp = getTemperatureInCelcius();
    printf("Temperature: %3.2f[*C] \r\n", temp);
}
