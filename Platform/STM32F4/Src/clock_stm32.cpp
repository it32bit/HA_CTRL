

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
            m_errorHandler();  // Call user-defined error handler
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
            m_errorHandler();  // Call user-defined error handler
    }
}

// static void SystemClock_Config(void)
// {
//     RCC_OscInitTypeDef RCC_OscInitStruct = {};
//     RCC_ClkInitTypeDef RCC_ClkInitStruct = {};

//     /** Configure the main internal regulator output voltage */
//     __HAL_RCC_PWR_CLK_ENABLE();
//     __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

//     /** Initializes the RCC Oscillators according to the specified parameters
//     in the RCC_OscInitTypeDef structure. */
//     RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
//     RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
//     RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
//     RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
//     RCC_OscInitStruct.PLL.PLLM       = 8;
//     RCC_OscInitStruct.PLL.PLLN       = 336;
//     RCC_OscInitStruct.PLL.PLLP       = RCC_PLLP_DIV2;
//     RCC_OscInitStruct.PLL.PLLQ       = 7;

//     if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//     {
//         Error_App_Handler();
//     }

//     /** Initializes the CPU, AHB and APB buses clocks */
//     RCC_ClkInitStruct.ClockType =
//         RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
//     RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
//     RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
//     RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
//     RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

//     if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
//     {
//         Error_App_Handler();
//     }
// }
