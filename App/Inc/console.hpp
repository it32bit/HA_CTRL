#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <stdint-gcc.h>
#include <stddef.h>

constexpr size_t CONSOLE_BUFFER_SIZE{128};
constexpr size_t CONSOLE_COMMAND_SIZE{5};

constexpr size_t MaxCommandNameLength = 16;
constexpr size_t MaxCommandDescLength = 64;

class Console
{
  public:
    static void send(const char* param);
    void        receivedData(uint8_t byte) noexcept;

    static void help(const char* param);
    static void reset(const char* param);
    static void echo(const char* param);
    static void temperature(const char* msg);
    static void watchdogTest(const char* msg);

  private:
    static constexpr size_t MaxLength = CONSOLE_BUFFER_SIZE;

    char   buffer[MaxLength]{};
    size_t m_head = 0;

    void process(const char* line);
    void cleanMessage() noexcept;
    void setMessageToProcess() noexcept;
    bool isBufferFull() const noexcept;
    bool isBufferEmpty() const noexcept;
    bool isMessageEnded(uint8_t byte) const noexcept;
    bool push(uint8_t byte) noexcept;
};

/**
 * @brief ConsoleCommand
 * @note If it is used std::function<void(std::string_view)> handler;
 *        Now as a handler it is able to use lambdas, member functions, or free functions.
 *
 * But: To create a constexpr-safe command table suitable for storing in flash memory
 * (especially in embedded systems like STM32), we need to avoid runtime-managed types
 * like std::function, std::vector, and even std::string_view
 * if it's not used carefully.
 *
 * 1. Use raw function pointers instead of std::function
 * 2. Use fixed-size C-style strings instead of std::string_view
 * 3. Define a literal-friendly struct
 * 4. Declare the command table as constexpr
 */

using CommandHandler = void (*)(const char*);

struct ConsoleCommand
{
    size_t         number;
    char           name[MaxCommandNameLength];
    CommandHandler handler;
    char           description[MaxCommandDescLength];
};

/**
 * @note This is the modern C++20 way to share constexpr data across files.
 */
inline constexpr ConsoleCommand cmdConfigArray[CONSOLE_COMMAND_SIZE] = {
    {0, "help",     &Console::help,         "Show available commands"  },
    {1, "reset",    &Console::reset,        "Reset the MCU"            },
    {2, "echo",     &Console::echo,         "Echo a number back"       },
    {3, "temp",     &Console::temperature,  "Get Temp from Tsensor"    },
    {4, "watchdog", &Console::watchdogTest, "Watchdog Test: while(1){}"},
};

#endif // _CONSOLE_HPP_
