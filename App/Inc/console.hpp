#ifndef _CONSOLE_HPP_
#define _CONSOLE_HPP_

#include <cstdint>
#include "circular_buffer.hpp"

constexpr size_t CONSOLE_BUFFER_SIZE{128};

extern void onByteReceived(uint8_t t_item);

#endif // _CONSOLE_HPP_
