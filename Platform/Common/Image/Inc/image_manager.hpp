#ifndef IMAGE_MANAGER_HPP
#define IMAGE_MANAGER_HPP

#include <cstdint>
#include "flash_layout.hpp"
#include "pil_flash_writer.hpp"

class ImageManager
{
  public:
    explicit ImageManager(IFlashWriter* t_writer);

    void writeImage(std::uintptr_t t_image_src, std::uintptr_t t_image_dst,
                    std::size_t t_image_size);
    void writeMeta(std::uintptr_t t_image_src, std::uintptr_t t_image_dst,
                   std::size_t t_image_size);
    void clearImage(std::uintptr_t t_image_start, std::size_t t_image_size);

  private:
    IFlashWriter* m_writer;
};

bool isImageStaged(std::uintptr_t t_metadata);
bool isImageAuthentic(std::uintptr_t t_firmware, std::uintptr_t t_metadata);

#endif // IMAGE_MANAGER_HPP