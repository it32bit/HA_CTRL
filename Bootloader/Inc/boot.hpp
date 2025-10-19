#ifndef BOOT_HPP
#define BOOT_HPP

typedef struct
{
    uint8_t major;
    uint8_t minor;
} firmwareVersionS;

__attribute__((section(".firmware_version"), used)) constexpr firmwareVersionS FIRMWARE_VERSION = {
    .major = 0, .minor = 3};

#endif // BOOT_HPP
