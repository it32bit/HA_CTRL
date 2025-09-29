#ifndef APP_HPP__
#define APP_HPP__

#include <cstdint>
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
    SubjectWithDebouce(uint32_t debounceMs) : debouncer_(debounceMs) {}

    void notifyObserversWhenStable()
    {
        if (debouncer_.shouldTrigger() == true)
        {
            Subject::notifyObservers();
        }
    }

  private:
    Debouncer debouncer_;
};

extern SubjectWithDebouce exti0_Subject;

class UserButtonManager : public Observer
{
  public:
    UserButtonManager(Subject& subject);
    void notify() const override;
    void process();

  private:
    Subject&     subject_;
    mutable bool pending_ = false;
};

class LedManager : public Observer
{
  public:
    LedManager(Subject& subject, const PinController& led);
    void notify() const override;
    void process();

  private:
    Subject&      subject_;
    PinController led_;
    mutable bool  pending_ = false;
};

#endif // APP_HPP__
