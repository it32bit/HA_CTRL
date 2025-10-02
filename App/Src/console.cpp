#include <cstring>
#include "console.hpp"
#include "stm32f4xx.h"

static void helpConsole(const char* t_msg);

void Console::receivedData(uint8_t t_item) noexcept
{
    if (isBufferFull())
    {
        cleanMessage();
    }

    if (isMessageEnded(t_item))
    {
        if (!isBufferEmpty())
        {
            setMessageToProcess();
            process(m_buffer.data());
            cleanMessage();
        }
    }
    else
    {
        push(t_item);
    }
}

void Console::send(const char* msg) noexcept
{
    while (*msg)
    {
        while (!(USART2->SR & USART_SR_TXE))
            ;
        USART2->DR = *msg++;
    }
}

// clang-format off
constexpr std::array<ConsoleCommandStructure<const char*>, CONSOLE_COMMAND_SIZE> cmdConfigArray =
{{
    {0, "help",  helpConsole, "Show available commands"},
    {1, "reset", nullptr, "Reset the MCU"          },
    {2, "echo",  nullptr, "Echo a number back"     }
}};
// clang-format on

void Console::process(const char* t_line)
{
    std::string_view input(t_line);

    for (const auto& cmd : cmdConfigArray)
    {
        if (input.substr(0, cmd.name.size()) == cmd.name)
        {
            const char* param = t_line + cmd.name.size();
            while (*param == ' ')
                ++param; // skip spaces

            cmd.handler(param);
            return;
        }
    }

    send("Unknown command\r\n");
}

static void helpConsole(const char* t_msg)
{
    std::string_view input(t_msg);

    for (const auto& cmd : cmdConfigArray)
    {
        if (input.substr(0, cmd.name.size()) == cmd.name)
        {
            Console::send(cmd.name.data());
            Console::send(" - ");
            Console::send(cmd.description.data());
            Console::send("\r\n");
            break;
        }
        else if (!input.size())
        {
            Console::send(cmd.name.data());
            Console::send(" - ");
            Console::send(cmd.description.data());
            Console::send("\r\n");
        }
    }
}