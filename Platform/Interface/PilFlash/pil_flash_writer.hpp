// Platform/Interface/pil_flash_writer.hpp

#ifndef PIL_FLASH_WRITER_HPP
#define PIL_FLASH_WRITER_HPP

#include <cstdint>

class IFlashWriter
{
  public:
    virtual void eraseSector(std::uint8_t t_sector)                        = 0;
    virtual void writeWord(std::uintptr_t t_address, std::uint32_t t_data) = 0;
    virtual void writeImage(std::uintptr_t t_address_src, std::uintptr_t t_address_dst,
                            std::size_t t_wordCount)                       = 0;
    virtual ~IFlashWriter()                                                = default;
};

#endif // PIL_FLASH_WRITER_HPP