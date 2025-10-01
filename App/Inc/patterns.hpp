#ifndef _DESIGN_PATTERNS_
#define _DESIGN_PATTERNS_

#include <list>
#include <array>
#include <mutex>

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
 * @brief Design Patterns: SINGLETON
 */
// class Singleton
// {
//   private:
//     static Singleton* m_pinstance;

//   protected:
//     std::string m_value;

//     Singleton(const std::string value) : m_value(value) {}

//     ~Singleton() {}

//   public:
//     Singleton(Singleton& other)  = delete;
//     Singleton(Singleton&& other) = delete;

//     void operator=(const Singleton&) = delete;
//     void operator=(Singleton&&)      = delete;

//     std::string value() const { return m_value; }

//     /**
//      * This is the static method that controls the access to the singleton
//      * instance. On the first run, it creates a singleton object and places it
//      * into the static field. On subsequent runs, it returns the client existing
//      * object stored in the static field.
//      */

//     static Singleton* getInstance(const std::string& value)
//     {
//         if (m_pinstance == nullptr)
//         {
//             m_pinstance = new Singleton(value);
//         }
//         return m_pinstance;
//     }

//     /**
//      * Finally, any singleton should define logic, which can be
//      * executed on its instance.
//      */
//     void logic();
// };

#endif // _DESIGN_PATTERNS_
