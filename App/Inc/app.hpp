#ifndef APP_HPP__
#define APP_HPP__

#include "patterns.hpp"
#include "api_gpio.hpp"

typedef struct
{
    uint8_t major;
    uint8_t minor;
} firmwareVersionS;

constexpr firmwareVersionS FIRMWARE_VERSION = {.major = 0, .minor = 1};

class Debouncer
{
  public:
    explicit Debouncer(uint32_t debounceMs);
    bool shouldTrigger();

  private:
    uint32_t debounceTime;
    uint32_t lastTick;
    bool     locked;
};

class SubjectWithDebouce : public Subject
{
  public:
    explicit SubjectWithDebouce(uint32_t t_debounce_ms) : m_debouncer(t_debounce_ms) {}

    void notifyObserversWhenStable(uint32_t mask)
    {
        if (m_debouncer.shouldTrigger() == true)
        {
            Subject::notifyObservers(mask);
        }
    }

  private:
    Debouncer m_debouncer;
};

extern SubjectWithDebouce exti0_Subject;

class UserButtonManager : public Observer
{
  public:
    UserButtonManager(Subject& t_subject, uint32_t t_pin_mask);
    void notify(uint32_t mask) const override;
    void process();

  private:
    Subject&              m_subject;
    uint32_t              m_pin_mask = -1;
    mutable volatile bool m_pending  = false;
    uint32_t              m_press_counter{0};
};

class LedManager : public Observer
{
  public:
    LedManager(Subject& t_subject, uint32_t t_pin_mask, const PinController& t_led);
    void notify(uint32_t mask) const override;
    void process();

  private:
    Subject&              m_subject;
    uint32_t              m_pin_mask = -1;
    PinController         m_led;
    mutable volatile bool m_pending{false};
};

void consoleNotify(uint8_t t_item);

inline constexpr StaticObserver<uint8_t> staticObservers[] = {{consoleNotify}};

inline constexpr StaticObserverList<uint8_t>
    uart2_Observers(staticObservers, sizeof(staticObservers) / sizeof(staticObservers[0]));

#endif // APP_HPP__
