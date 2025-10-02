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
 * @brief Design Pattern: Observer Static Interface
 */

template <typename T>
struct StaticObserver
{
    void (*onNotify)(T);
};

template <typename T, size_t N>
class StaticObserverList
{
  public:
    constexpr StaticObserverList(std::array<StaticObserver<T>, N> t_obs) : m_observers(t_obs) {}

    void notifyAll(T value) const
    {
        for (const auto& obs : m_observers)
        {
            if (obs.onNotify)
            {
                obs.onNotify(value);
            }
        }
    }

  private:
    std::array<StaticObserver<T>, N> m_observers;
};

#endif // _DESIGN_PATTERNS_
