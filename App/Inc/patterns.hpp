#ifndef _DESIGN_PATTERNS_
#define _DESIGN_PATTERNS_

#include <list>
#include <cstdint>

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

#endif // _DESIGN_PATTERNS_
