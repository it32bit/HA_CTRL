#include "uart_receiver_stm32.hpp"

UartReceiver::UartReceiver(IConsoleUart& uart, FlashWriterSTM32F4& writer)
    : m_uart(uart), m_writer(writer)
{
}

void UartReceiver::receiveImage(std::uintptr_t flashDest, std::size_t imageSize)
{
    std::uint8_t buffer[4];
    std::size_t  received = 0;

    while (received < imageSize)
    {
        std::size_t chunk = 0;

        // Read up to 4 bytes
        while (chunk < 4 && received + chunk < imageSize)
        {
            char byte;
            if (m_uart.read(byte))
            {
                buffer[chunk++] = static_cast<std::uint8_t>(byte);
            }
        }

        // Pad remaining bytes with 0xFF if imageSize isn't word-aligned
        while (chunk < 4)
        {
            buffer[chunk++] = 0xFF;
        }

        std::uint32_t word = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);

        m_writer.writeWord(flashDest, word);
        flashDest += 4;
        received += chunk;
    }
}
