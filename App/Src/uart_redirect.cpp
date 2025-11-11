
#include "uart_manager_stm32.hpp"

static UartManager* g_uartManager = nullptr;

void setUartRedirect(UartManager& manager)
{
    g_uartManager = &manager;
}

extern "C" int _write(int file, char* ptr, int len)
{
    if (!g_uartManager) return 0;

    for (int i = 0; i < len; ++i)
    {
        g_uartManager->write(ptr[i]);
    }
    return len;
}