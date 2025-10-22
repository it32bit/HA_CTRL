/**
 * @file      Boot/Inc/boot_flag_manager.hpp
 * @author    it32bit
 * @brief     Manages persistent boot state across resets using flash-backed flags.
 *            Tracks firmware update progression (staged, verified, applied)
 *            to coordinate BootPrim → BootSec → Application transitions.
 *
 * @version   1.0
 * @date      2025-10-23
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef BOOT_FLAG_MANAGER_HPP
#define BOOT_FLAG_MANAGER_HPP

#include <cstdint>
#include "flash_layout.hpp"
#include "pil_flash_writer.hpp"

// ASCII-encoded 4-character status markers for flash-backed boot state
enum class BootState : std::uint32_t
{
    Idle     = 0x00000000,
    Staged   = 0x53544147, // 'STAG'
    Verified = 0x56455249, // 'VERI'
    Applied  = 0x4150504C, // 'APPL'
    Failed   = 0x4641494C, // 'FAIL'
    BLANK    = 0xFFFFFFFF
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
    static constexpr std::uintptr_t FLAG_ADDR = FlashLayout::CONFIG_START;
    static constexpr std::uint8_t   CONFIG_SECTOR =
        FlashLayout::sectorFromAddress(FLAG_ADDR); // Adjust based on address
};

#endif // BOOT_FLAG_MANAGER_HPP