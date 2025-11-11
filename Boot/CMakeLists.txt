# Boot/CMakeLists.txt

add_library(pil_boot INTERFACE)

target_include_directories(pil_boot INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
)
