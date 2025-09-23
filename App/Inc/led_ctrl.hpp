/**
 ******************************************************************************
 * @file           : ha_ctrl.hpp
 * @brief          : Header for ha_ctrl.cpp file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 */
#ifndef __LED_CTRL_HPP
#define __LED_CTRL_HPP

#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"

/**
 * Simple LED controller class
 */
class LedController
{
  public:
    LedController(GPIO_TypeDef* port, uint16_t pin) : port_(port), pin_(pin) {}

    void toggle() { LL_GPIO_TogglePin(port_, pin_); }

    void on() { LL_GPIO_SetOutputPin(port_, pin_); }

    void off() { LL_GPIO_ResetOutputPin(port_, pin_); }

  private:
    GPIO_TypeDef* port_;
    uint16_t      pin_;
};

#endif /* __HA_CTRL_HPP */