#ifndef PIL_GPIO_HPP
#define PIL_GPIO_HPP

#include <cstdint>
#include <string_view>

class IGPIOPin
{
  public:
    virtual ~IGPIOPin() = default;

    virtual void toggle()     = 0;
    virtual void set()        = 0;
    virtual void reset()      = 0;
    virtual bool read() const = 0;

    virtual std::string_view name() const = 0;
};

class IGPIOManager
{
  public:
    virtual ~IGPIOManager() = default;

    // Get a pin controller by name. Return nullptr if not found.
    virtual IGPIOPin* getPin(std::string_view pinName) = 0;

    // std::optional<IGPIOPin*> getPin(std::string_view t_name);
};

#endif // PIL_GPIO_HPP
