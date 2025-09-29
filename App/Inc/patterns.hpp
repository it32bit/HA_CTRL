#ifndef _DESIGN_PATTERNS_
#define _DESIGN_PATTERNS_

#include <list>
#include <cstdint>

class Observer
{
  public:
    virtual void notify() const = 0;

    virtual ~Observer() = default;
};

class Subject
{
  public:
    void registerObserver(Observer* observer) { observers.push_back(observer); }

    void unregisterObserver(Observer* observer) { observers.remove(observer); }

    void notifyObservers() const
    {
        for (auto observer : observers)
            observer->notify();
    }

  private:
    std::list<Observer*> observers;
};

#endif // _DESIGN_PATTERNS_
