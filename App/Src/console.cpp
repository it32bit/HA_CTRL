#include "console.hpp"
#include "stm32f4xx.h"

void onByteReceived(uint8_t t_item)
{
    // Example: echo back
    USART2->DR = t_item;
}