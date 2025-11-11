# Platform/Interface/PilWatchdog/CMakeLists.txt

add_library(pil_watchdog INTERFACE)

target_include_directories(pil_watchdog INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
