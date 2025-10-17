# Platform/Interface/PilGpio/CMakeLists.txt

add_library(pil_gpio INTERFACE)

target_include_directories(pil_gpio INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
