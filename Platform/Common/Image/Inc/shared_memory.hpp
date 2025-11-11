#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <cstdint>

namespace Shared
{

extern volatile std::uint32_t firmwareUpdateFlag;

constexpr std::uint32_t PREPARE_TO_RECEIVE_BINARY = 0xFEEDC0DE;

} // namespace Shared

#endif // SHARED_MEMORY_HPP
