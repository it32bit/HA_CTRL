// flash_writer_stm32.hpp

#ifndef FLASH_WRITER_HPP
#define FLASH_WRITER_HPP

#include "pil_flash_writer.hpp"

class FlashWriterSTM32F4 : public IFlashWriter
{
  public:
    void unlock() override;
    void eraseSector(std::uint8_t t_sector) override;
    void writeWord(std::uintptr_t t_address, std::uint32_t t_data) override;
};

#endif // FLASH_WRITER_HPP
