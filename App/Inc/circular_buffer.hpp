#ifndef CIRCULAR_BUFFER_HPP
#define CIRCULAR_BUFFER_HPP

#include <string>
#include <array>
#include <functional>

template <class T, size_t TSize, size_t TObserverCount>
class CircularBuffer
{
  public:
    static CircularBuffer& instance()
    {
        static CircularBuffer circularBuffer;
        return circularBuffer;
    }

    bool push(const T t_item)
    {
        auto next = (m_head + 1) % TSize;
        if (next == m_tail)
        {
            m_full = true;
            return false; // buffer full
        }
        m_buff[m_head] = t_item;
        m_head         = next;
        notifyObservers(t_item);
        return true;
    }

    bool pop(T& t_item)
    {
        if (m_head == m_tail)
        {
            return false; // buffer empty
        }
        t_item = m_buff[m_tail];
        m_tail = (m_tail + 1) % TSize;
        return true;
    }

    void reset() noexcept
    {
        m_head = m_tail;
        m_full = false;
    }

    size_t size() const noexcept
    {
        size_t size = TSize;

        if (m_head >= m_tail)
        {
            size = m_head - m_tail;
        }
        else
        {
            size = TSize + m_head - m_tail;
        }
        return size;
    }

    bool empty() const noexcept
    {
        return (m_head == m_tail); // if head and tail are equal, we are empty
    }

    bool full() const noexcept
    {
        return m_full; // If tail is ahead the head by 1, we are full
    }

    size_t capacity() const noexcept
    {
        return TSize; // Return array size
    }

    void registerObserver(std::function<void(T)> callback)
    {
        observers[m_observer_count++] = callback; // Add new observer
    }

    auto getData() { return m_buff.data(); }

  private:
    CircularBuffer() = default;

    std::array<T, TSize> m_buff;

    volatile size_t m_head = 0;
    volatile size_t m_tail = 0;
    mutable bool    m_full = false;
    size_t          m_observer_count{};
    size_t          m_observer_active = 0;

    static constexpr size_t MaxObservers{TObserverCount};

    // void (*observers[MaxObservers])(T) = {};
    std::function<void(T)> observers[MaxObservers];

    void notifyObservers(T t_item)
    {
        for (size_t i = 0; i < m_observer_count; ++i)
        {
            observers[i](t_item);
        }
    }
};
#endif // CIRCULAR_BUFFER_HPP
