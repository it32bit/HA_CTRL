#ifndef _BOOT_HPP_
#define _BOOT_HPP_

typedef struct
{
    uint8_t major;
    uint8_t minor;
} firmwareVersionS;

__attribute__((section(".firmware_version"),
               used)) constexpr firmwareVersionS FIRMWARE_VERSION = {.major = 0, .minor = 3};

#endif // _BOOT_HPP_