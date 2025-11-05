#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <cstdint>

namespace Shared
{

alignas(4) __attribute__((section(".shared_ram"))) std::uint32_t firmwareUpdateFlag = 0;

constexpr std::uint32_t PREPARE_TO_RECEIVE_BINARY = 0xFEEDC0DE;

} // namespace Shared

#endif // SHARED_MEMORY_HPP