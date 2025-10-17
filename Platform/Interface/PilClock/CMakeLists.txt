# Platform/Interface/PilClock/CMakeLists.txt

add_library(pil_clock INTERFACE)

target_include_directories(pil_clock INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
