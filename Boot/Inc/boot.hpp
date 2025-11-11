#ifndef BOOT_HPP
#define BOOT_HPP

#include <cstdint>

struct FirmwareVersion
{
    std::uint8_t major;
    std::uint8_t minor;
} __attribute__((packed));

class Bootloader
{
  public:
    static void jumpToAddress(const std::uintptr_t addr);
    static void deinitPeripherals();

  private:
    Bootloader() = delete;
};

#endif // BOOT_HPP
