// boot_sec.hpp

#ifndef BOOT_SEC_HPP
#define BOOT_SEC_HPP

#include "boot.hpp"
// Define the instance in a specific section (used for linking, bootloader, etc.)
__attribute__((section(".firmware_version"), used)) constexpr FirmwareVersion FIRMWARE_VERSION = {
    .major = 0, .minor = 5};

#endif // BOOT_SEC_HPP
