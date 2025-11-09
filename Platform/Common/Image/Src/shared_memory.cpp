#include "shared_memory.hpp"

namespace Shared
{

alignas(4) __attribute__((section(".shared_ram"))) volatile std::uint32_t firmwareUpdateFlag;

}
