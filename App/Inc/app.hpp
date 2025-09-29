#ifndef APP_HPP__
#define APP_HPP__

#include <cstdint>

typedef struct
{
    uint8_t major;
    uint8_t minor;
} firmwareVersionS;

constexpr firmwareVersionS FIRMWARE_VERSION = {.major = 0, .minor = 1};

#endif // APP_HPP__