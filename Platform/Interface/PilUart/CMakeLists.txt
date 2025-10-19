# Platform/Interface/PilUart/CMakeLists.txt

add_library(pil_uart INTERFACE)

target_include_directories(pil_uart INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
