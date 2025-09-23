/**
 ******************************************************************************
 * @file           : ha_ctrl.hpp
 * @brief          : Header for ha_ctrl.cpp file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 */
#ifndef __HA_CTRL_HPP
#define __HA_CTRL_HPP

#include "stm32f4xx_hal.h"

/**
 * Simple LED controller class
 */
class LedController
{
public:
    LedController(GPIO_TypeDef *port, uint16_t pin)
        : port_(port), pin_(pin) {}

    void toggle()
    {
        HAL_GPIO_TogglePin(port_, pin_);
    }
    void on()
    {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
    }
    void off()
    {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
    }

private:
    GPIO_TypeDef *port_;
    uint16_t pin_;
};

#endif /* __HA_CTRL_HPP */