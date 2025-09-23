/**
 ******************************************************************************
 * @file           : ha_ctrl.cpp
 * @author         : i32bit
 * @brief          : High-level application control logic
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 */
#include "led_ctrl.hpp"
#include "ha_ctrl.hpp"

static void AppInit_cpp();

/**
 * Main application entry point for C++ code
 */
extern "C" void App_cpp(void)
{
    /** Initialization code for C++ application can be added here */
    AppInit_cpp();

    LedController led(GPIOD, GPIO_PIN_12);

    led.on();

    /** Main loop */
    for (;;)
    {
    }
}

/**
 * @brief  Heatbeat LDx Led Toggle
 */
extern "C" void HeartBeat_SysTick(void)
{
    static uint32_t ticks = 0;
    if (ticks++ >= 500) // Toggle every 500ms
    {
        ticks = 0;
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13); // Toggle LD3
    }
}

/**
 * Initialization function for C++ application
 */
static void LD4_Init(void);
static void LD3_Init(void);

static void AppInit_cpp()
{
    LD4_Init();
    LD3_Init();
    /** Any initialization code can be added here */
}

static void LD3_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = GPIO_PIN_13,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
        .Alternate = 0};
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void LD4_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {
        .Pin = GPIO_PIN_12,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
        .Alternate = 0};
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}
