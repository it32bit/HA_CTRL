#ifndef BOOT_HPP
#define BOOT_HPP

#include "pil_flash_writer.hpp"
#include "firmware_metadata.hpp"

struct FirmwareVersion
{
    std::uint8_t major;
    std::uint8_t minor;
} __attribute__((packed));

// Define the instance in a specific section (used for linking, bootloader, etc.)
__attribute__((section(".firmware_version"), used)) constexpr FirmwareVersion FIRMWARE_VERSION = {
    .major = 0, .minor = 3};

class Bootloader
{
  public:
    explicit Bootloader(IFlashWriter* writer);

    bool isMetadataValid() const;
    bool verifyStagedFirmware() const;
    void applyUpdate();
    void jumpToApplication();

  private:
    IFlashWriter*             m_flash;
    const Firmware::Metadata& m_metadata() const;
};

#endif // BOOT_HPP
