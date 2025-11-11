#ifndef UART_RECEIVER_HPP
#define UART_RECEIVER_HPP

#include <cstdint>
#include <cstddef>
#include "pil_uart.hpp"
#include "flash_writer_stm32.hpp"

class UartReceiver {
  public:
    UartReceiver(IConsoleUart& uart, FlashWriterSTM32F4& writer);

    void receiveImage(std::uintptr_t flashDest, std::size_t imageSize);

  private:
    IConsoleUart& m_uart;
    FlashWriterSTM32F4& m_writer;
};

#endif // UART_RECEIVER_HPP
