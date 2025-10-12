#ifndef _BOOT_HPP_
#define _BOOT_HPP_

#ifdef __cplusplus

    #define APP_ADDRESS 0x08004000
    #define VTOR (*(volatile uint32_t*)0xE000ED08)

extern "C"
{
#endif

    // "C" place here

#ifdef __cplusplus
}
#endif

#endif // _BOOT_HPP_