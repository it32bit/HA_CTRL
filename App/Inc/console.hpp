#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <cstdint>
#include <stddef.h>

constexpr size_t CONSOLE_BUFFER_SIZE{128};

extern void onByteReceived(uint8_t byte);

template <size_t Size>
class UartBufferSingleton
{
  public:
    static UartBufferSingleton& instance()
    {
        static UartBufferSingleton buffer;
        return buffer;
    }

    bool push(uint8_t byte)
    {
        size_t next = (head + 1) % Size;
        if (next == tail)
        {
            return false; // buffer full
        }
        data[head] = byte;
        head       = next;
        notifyObservers(byte);
        return true;
    }

    bool pop(uint8_t& byte)
    {
        if (head == tail)
            return false; // buffer empty
        byte = data[tail];
        tail = (tail + 1) % Size;
        return true;
    }

    void registerObserver(void (*callback)(uint8_t)) { observers[observerCount++] = callback; }

  private:
    UartBufferSingleton() = default;
    uint8_t         data[Size];
    volatile size_t head = 0;
    volatile size_t tail = 0;

    static constexpr size_t MaxObservers{4};
    void (*observers[MaxObservers])(uint8_t) = {};
    size_t observerCount{};

    void notifyObservers(uint8_t byte)
    {
        for (size_t i = 0; i < observerCount; ++i)
        {
            observers[i](byte);
        }
    }
};

#endif // _CONSOLE_HPP_