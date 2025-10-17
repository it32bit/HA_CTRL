#ifndef PIL_CLOCK_CONFIGURATOR_HPP
#define PIL_CLOCK_CONFIGURATOR_HPP

class IClockConfigurator
{
  public:
    virtual ~IClockConfigurator()                       = default;
    virtual void configure(void (*handler)() = nullptr) = 0;
};

#endif // PIL_CLOCK_CONFIGURATOR_HPP
