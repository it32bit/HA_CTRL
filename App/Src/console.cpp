#include "console.hpp"
#include "stm32f4xx.h"

void onByteReceived(uint8_t byte)
{
    // Example: echo back
    USART2->DR = byte;
}