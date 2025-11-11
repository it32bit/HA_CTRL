/**
 ******************************************************************************
 * @file        clock_stm32.cpp
 * @author      it32bit
 * @brief       Implements the Clock_STM32F4 class for configuring the system clock on STM32F4 devices.
 *              Uses the STM32 HAL to initialize HSE and PLL settings, and applies clock dividers for AHB and APB buses.
 *              Supports optional error handling via user-provided callback.
 *
 * @version     1.0
 * @date        2025-10-19
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#include "clock_stm32.hpp"
#include "stm32f4xx_hal.h"

void Clock_STM32F4::configure(void (*handler)())
{
    m_errorHandler = handler;
    configureSystemClock();
}

void Clock_STM32F4::configureSystemClock()
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM       = 8;
    RCC_OscInitStruct.PLL.PLLN       = 336;
    RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ       = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        if (m_errorHandler)
            m_errorHandler(); // Call user-defined error handler
    }

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        if (m_errorHandler)
            m_errorHandler(); // Call user-defined error handler
    }
}
