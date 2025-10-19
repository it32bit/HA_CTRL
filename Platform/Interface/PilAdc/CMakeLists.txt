# Platform/Interface/PilAdc/CMakeLists.txt

add_library(pil_adc INTERFACE)

target_include_directories(pil_adc INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
