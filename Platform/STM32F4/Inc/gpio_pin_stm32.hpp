#include "i_gpio.hpp"
#include "stm32f4xx_hal.h"
#include <string>

class GpioPin_STM32 : public IGPIOPin
{
  public:
    GpioPin_STM32(GPIO_TypeDef* port, uint16_t pin, std::string_view pinName);

    void toggle() override;
    void set() override;
    void reset() override;
    bool read() const override;

    std::string_view name() const override { return m_name; }

  private:
    GPIO_TypeDef* m_port;
    uint16_t      m_pin;
    std::string   m_name;
};
