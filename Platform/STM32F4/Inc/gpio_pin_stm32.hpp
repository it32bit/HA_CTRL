#include "pil_gpio.hpp"
#include "stm32f4xx_hal.h"
#include <string_view>

class GpioPin_STM32 : public IGPIOPin
{
  public:
    GpioPin_STM32(std::string_view name, GPIO_TypeDef* port, uint16_t pin);

    void toggle() override;
    void set() override;
    void reset() override;
    bool read() const override;

    std::string_view name() const override;

  private:
    GPIO_TypeDef*    m_port;
    uint16_t         m_pin_mask;
    std::string_view m_name;
};
