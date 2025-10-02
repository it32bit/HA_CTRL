#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <cstdint>
#include "circular_buffer.hpp"

constexpr size_t CONSOLE_BUFFER_SIZE{128};
constexpr size_t CONSOLE_COMMAND_SIZE{3};

template <typename T>
struct ConsoleCommandStructure
{
    size_t           number;
    std::string_view name;
    void (*handler)(T);
    std::string_view description;
};

class Console
{
  public:
    void receivedData(uint8_t t_item) noexcept;

    static void send(const char* msg) noexcept;

  private:
    static constexpr size_t        m_max_length{CONSOLE_BUFFER_SIZE};
    std::array<char, m_max_length> m_buffer;
    size_t                         m_head{};

    void process(const char* line);

    void setMessageToProcess() { m_buffer[m_head] = '\0'; }

    bool isBufferFull() const noexcept { return m_head == m_max_length - 1; }

    bool isBufferEmpty() const noexcept { return m_head == 0; }

    bool isMessageEnded(const uint8_t t_item) const noexcept
    {
        if (t_item == '\r' || t_item == '\n')
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool push(const uint8_t t_item)
    {
        m_buffer[m_head++] = t_item;
        return true;
    }

    void cleanMessage()
    {
        m_head      = 0;
        m_buffer[0] = '\0';
    }
};

template <class T, size_t TObserverCount>
class ConsolObserver
{
  public:
    static ConsolObserver& instance()
    {
        static ConsolObserver consolObserver;
        return consolObserver;
    }

    void registerObserver(std::function<void(T)> callback)
    {
        observers[m_observer_count++] = callback; // Add new observer
    }

    void notifyObservers(T t_item)
    {
        for (size_t i = 0; i < m_observer_count; ++i)
        {
            observers[i](t_item);
        }
    }

  private:
    ConsolObserver() = default;

    size_t                  m_observer_count{};
    static constexpr size_t MaxObservers{TObserverCount};
    std::function<void(T)>  observers[MaxObservers];
};

#endif // _CONSOLE_HPP_
