#include <stdint-gcc.h>
#include "api_debug.hpp"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_rcc.h"

static uint32_t getAPB1ClockFreq(void);

void uart2_send_char_cpp(char c)
{
    while (!(USART2->SR & USART_SR_TXE))
    {
    }; // Wait until TX buffer is empty
    USART2->DR = c;
}

void uart2_send_string(const char* str)
{
    while (*str != '\0')
    {
        uart2_send_char_cpp(*str++);
    }
}

void uart2_send_char(char c)
{
    while (!(USART2->SR & USART_SR_TXE))
    {
    }; // Wait until TX buffer is empty
    USART2->DR = c;
}

extern "C" int _write(int file, char* ptr, int len)
{
    for (int i = 0; i < len; i++)
    {
        uart2_send_char(ptr[i]);
    }
    return len;
}

// void debugInit(USART_TypeDef *usart)
void debugInit(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();

    uint32_t brr = getAPB1ClockFreq() / 115200;

    USART2->CR1 = 0; // Disable USART before config
    USART2->BRR = brr;

    USART2->CR1 |= USART_CR1_TE; // Enable transmitter
    USART2->CR1 |= USART_CR1_UE; // Enable USART
    USART2->CR1 |= USART_CR1_RE;
    USART2->CR1 |= USART_CR1_RXNEIE;

    NVIC_EnableIRQ(USART2_IRQn);
}

extern "C" uint32_t LL_RCC_GetSysClkSource(void);

static uint32_t getAPB1ClockFreq(void)
{
    uint32_t sysclk = 0;

    switch (LL_RCC_GetSysClkSource())
    {
        case LL_RCC_SYS_CLKSOURCE_STATUS_HSI:
            sysclk = HSI_VALUE;
            break;
        case LL_RCC_SYS_CLKSOURCE_STATUS_HSE:
            sysclk = HSE_VALUE;
            break;
        case LL_RCC_SYS_CLKSOURCE_STATUS_PLL:
        {
            // Extract PLL parameters from RCC->PLLCFGR
            uint32_t pllm      = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> RCC_PLLCFGR_PLLM_Pos;
            uint32_t plln      = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos;
            uint32_t pllp_bits = (RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >> RCC_PLLCFGR_PLLP_Pos;
            uint32_t pllp      = (pllp_bits + 1) * 2;

            // Determine PLL source
            uint32_t pll_source = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
            uint32_t pll_input  = (pll_source == RCC_PLLCFGR_PLLSRC_HSE) ? HSE_VALUE : HSI_VALUE;

            // Compute system clock from PLL
            uint32_t vco_input  = pll_input / pllm;
            uint32_t vco_output = vco_input * plln;
            sysclk              = vco_output / pllp;
            break;
        }
        default:
            sysclk = HSI_VALUE;
            break;
    }
    uint32_t presc = APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos];
    return sysclk >> presc;
}
