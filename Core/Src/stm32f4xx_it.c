/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_it.hpp"
#include "stm32f4xx_it.h"

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
    while (1)
    {
        ;
    }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    while (1)
    {
        ;
    }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
    while (1)
    {
        ;
    }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
    while (1)
    {
        ;
    }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
    while (1)
    {
        ;
    }
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
    ;
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick(); // Required if using HAL
    HeartBeat_SysTick();
}

/**
 * @brief This function handles External Interrupts
 */
void EXTI0_IRQHandler(void)
{
    if (LL_EXTI_IsActiveFlag_0_31(GPIO_PIN_0) != RESET)
    {
        LL_EXTI_ClearFlag_0_31(GPIO_PIN_0);
        EXTI0_Callback(GPIO_PIN_0);
    }
}

/**
 * @brief This function handles External Interrupts
 */
void USART2_IRQHandler(void)
{
    if (USART2->SR & USART_SR_RXNE)
    {
        volatile uint32_t data = USART2->DR; // Read clears RXNE
        USART2_Callback(data);
    }
}
