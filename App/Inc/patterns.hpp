#ifndef _DESIGN_PATTERNS_
#define _DESIGN_PATTERNS_

#include <list>
#include <array>
#include <functional>

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
        for (auto observer : m_observers)
        {
            observer->notify(t_data);
        }
    }

  private:
    std::list<Observer*> m_observers;
};

/**
 * @brief Design Patterns: Observer with Template
 *
 */
template <typename T, size_t MaxObservers>
class ObserverTemplate
{
  public:
    void registerObserver(std::invocable<T> auto&& callback)
    {
        if (count < MaxObservers)
        {
            observers[count++] = std::forward<decltype(callback)>(callback);
        }
    }

    void notify(T item)
    {
        for (size_t i = 0; i < count; ++i)
        {
            observers[i](item);
        }
    }

  private:
    std::array<std::function<void(T)>, MaxObservers> observers{};

    size_t count = 0;
};

#endif // _DESIGN_PATTERNS_
