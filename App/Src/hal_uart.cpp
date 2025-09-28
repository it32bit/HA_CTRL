#include "stm32f4xx.h"        // CMSIS core
#include "stm32f4xx_ll_rcc.h" // LL RCC macros
#include <stdio.h>

void uart2_send_char_cpp(char c)
{
    while (!(USART2->SR & USART_SR_TXE))
        ; // Wait until TX buffer is empty
    USART2->DR = c;
}

void uart2_send_string(const char* str)
{
    while (*str)
        uart2_send_char_cpp(*str++);
}

static uint32_t getAPB1ClockFreq(void);

void Init_Uart2(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();

    uint32_t brr = getAPB1ClockFreq() / 115200;

    USART2->CR1 = 0; // Disable USART before config
    USART2->BRR = brr;

    USART2->CR1 |= USART_CR1_TE; // Enable transmitter
    USART2->CR1 |= USART_CR1_UE; // Enable USART
}

extern "C" uint32_t LL_RCC_GetSysClkSource(void);
extern "C" uint32_t LL_RCC_GetAPB1Prescaler(void);

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
