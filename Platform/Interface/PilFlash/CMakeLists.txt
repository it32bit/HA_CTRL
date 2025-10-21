# Platform/Interface/PilFlash/CMakeLists.txt

add_library(pil_flash INTERFACE)

target_include_directories(pil_flash INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
