/**
 ******************************************************************************
 * @file           :
 ******************************************************************************
 */
#ifndef __API_GPIO_HPP
#define __API_GPIO_HPP

#include "hal_gpio.hpp"

/**
 * Simple PIN controller class
 */
class PinController
{
  public:
    PinController(GPIO_TypeDef* port, uint16_t pin) : port_(port), pin_(pin) {}

    void toggle() { LL_GPIO_TogglePin(port_, pin_); }

    void on() { LL_GPIO_SetOutputPin(port_, pin_); }

    void off() { LL_GPIO_ResetOutputPin(port_, pin_); }

  private:
    GPIO_TypeDef* port_;
    uint16_t      pin_;
};

#endif /* __API_GPIO_HPP */