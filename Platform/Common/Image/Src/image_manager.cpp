#include "image_manager.hpp"
#include "stm32f4xx.h"
#include "flash_layout.hpp"
#include "firmware_metadata.hpp"
#include "crc32_check.hpp"

ImageManager::ImageManager(IFlashWriter* writer) : m_writer(writer) {}

void ImageManager::writeImage(std::uintptr_t t_image_src, std::uintptr_t t_image_dst,
                              std::size_t t_image_size)
{
    __disable_irq();

    m_writer->unlock();

    // Image can be bigger then sector size
    std::uintptr_t current_addr = t_image_dst;
    std::uintptr_t end_addr     = t_image_dst + t_image_size;

    while (current_addr < end_addr)
    {
        uint8_t       sector = FlashLayout::sectorFromAddress(current_addr);
        std::uint32_t size   = FlashLayout::sectorSize(sector);

        m_writer->eraseSector(sector);
        current_addr += size;
    }

    m_writer->writeImage(t_image_src, t_image_dst, t_image_size);
    m_writer->lock();
    __enable_irq();
}

void ImageManager::writeMeta(std::uintptr_t t_image_src, std::uintptr_t t_image_dst,
                             std::size_t t_image_size)
{
    __disable_irq();

    m_writer->unlock();

    const Firmware::Metadata* metadata = reinterpret_cast<const Firmware::Metadata*>(t_image_dst);

    if ((metadata->magic == Firmware::METADATA_MAGIC) ||
        (metadata->magic != Firmware::METADATA_EMPTY))
    {
        // The image does not occupy this sector
        uint8_t sector = FlashLayout::sectorFromAddress(t_image_dst);
        m_writer->eraseSector(sector);
    }
    m_writer->writeImage(t_image_src, t_image_dst, t_image_size);
    m_writer->lock();
    __enable_irq();
}

void ImageManager::clearImage(std::uintptr_t t_image_start, std::size_t t_image_size)
{
    __disable_irq();

    m_writer->unlock();
    // Image can be bigger then sector size
    std::uintptr_t current_addr = t_image_start;
    std::uintptr_t end_addr     = t_image_start + t_image_size;

    while (current_addr < end_addr)
    {
        uint8_t       sector = FlashLayout::sectorFromAddress(current_addr);
        std::uint32_t size   = FlashLayout::sectorSize(sector);

        m_writer->eraseSector(sector);
        current_addr += size;
    }
    m_writer->lock();
    __enable_irq();
}

bool isImageStaged(std::uintptr_t t_metadata)
{
    const Firmware::Metadata* metadata = reinterpret_cast<const Firmware::Metadata*>(t_metadata);

    bool result = false;

    if (metadata->magic == Firmware::METADATA_MAGIC)
    {
        result = true;
    }

    return result;
}

bool isImageAuthentic(std::uintptr_t t_firmware, std::uintptr_t t_metadata)
{
    const std::uint8_t* firmware = reinterpret_cast<const std::uint8_t*>(t_firmware);

    const Firmware::Metadata* metadata = reinterpret_cast<const Firmware::Metadata*>(t_metadata);

    bool result = false;

    if (metadata->magic == Firmware::METADATA_MAGIC)
    {
        std::size_t   firmware_size = metadata->firmwareSize;
        std::uint32_t expected_crc  = metadata->firmwareCRC;

        result = Integrity::CRC32Checker::verify(firmware, firmware_size, expected_crc);
    }

    return result;
}
