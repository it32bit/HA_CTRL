// boot_flag_manager.hpp
/**
 * BootFlagManager class to track update status across resets.
 * This mechanism complements bootloader architecture and is essential for:
 *    - Tracking firmware update status (e.g., staged, verified, applied)
 *    - Supporting rollback or recovery
 *    - Coordinating BootPrim → BootSec → App transitions
 */

#ifndef BOOT_FLAG_MANAGER_HPP
#define BOOT_FLAG_MANAGER_HPP

#include <cstdint>
#include "flash_layout.hpp"
#include "pil_flash_writer.hpp"

enum class BootState : std::uint32_t
{
    // These values are four-character ASCII strings encoded as 32-bit integers
    Idle     = 0x00000000,
    Staged   = 0x53544147, // 'STAG'
    Verified = 0x56455249, // 'VERI'
    Applied  = 0x4150504C, // 'APPL'
    Failed   = 0x4641494C  // 'FAIL'
};

class BootFlagManager
{
  public:
    explicit BootFlagManager(IFlashWriter* writer);

    BootState getState() const;
    void      setState(BootState state);
    void      clear();

  private:
    IFlashWriter*                   m_writer;
    static constexpr std::uintptr_t FLAG_ADDR     = FlashLayout::CONFIG_START;
    static constexpr std::uint8_t   CONFIG_SECTOR = 11; // Adjust based on address
};

#endif // BOOT_FLAG_MANAGER_HPP