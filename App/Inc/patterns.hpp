#ifndef DESIGN_PATTERNS
#define DESIGN_PATTERNS

#include <list>
#include <stdint-gcc.h>

/**
 * @brief Design Patterns: Observer
 *
 * @note Contain classes: Observer, Subject
 */
class Observer
{
  public:
    virtual void notify(uint32_t t_mask) const = 0;

    virtual ~Observer() = default;
};

class Subject
{
  public:
    void registerObserver(Observer* t_observer) { m_observers.push_back(t_observer); }

    void unregisterObserver(Observer* t_observer) { m_observers.remove(t_observer); }

    void notifyObservers(uint32_t t_data) const
    {
        for (auto* observer : m_observers)
        {
            observer->notify(t_data);
        }
    }

  private:
    std::list<Observer*> m_observers;
};

/**
 * @brief Design Pattern: Observer Static Interface
 */

template <typename T>
struct StaticObserver
{
    void (*onNotify)(T);
};

template <typename T>
class StaticObserverList
{
  public:
    constexpr StaticObserverList(const StaticObserver<T>* t_obs, size_t t_count)
        : m_observers(t_obs), m_size(t_count)
    {
    }

    void notifyAll(T value) const noexcept
    {
        for (size_t i = 0; i < m_size; ++i)
        {
            if (m_observers[i].onNotify)
            {
                m_observers[i].onNotify(value);
            }
        }
    }

  private:
    const StaticObserver<T>* m_observers;

    size_t m_size;
};

#endif // DESIGN_PATTERNS
