# Platform/STM32F4/CMakeLists.txt

include("${CMAKE_SOURCE_DIR}/cmake/project-sources.cmake")

set(PLATFORM_TARGET Platform_STM32F4)

file(GLOB PLATFORM_SOURCES
    Src/*.cpp
)

add_library(${PLATFORM_TARGET} STATIC ${PLATFORM_SOURCES})

target_compile_options(${PLATFORM_TARGET} PRIVATE
    ${cpu_PARAMS}
)

target_include_directories(${PLATFORM_TARGET} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Platform/Interface
)

# Add HAL/CMSIS headers if needed (likely required)
target_include_directories(${PLATFORM_TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/cmsis-device-f4/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
)

# Compiler defines
target_compile_definitions(${PLATFORM_TARGET} PRIVATE
    USE_HAL_DRIVER
    ${MCU_DEFINES}
)

# Link CUSTOM_HAL/LL libraries here if needed
# target_link_libraries(${PLATFORM_TARGET} PRIVATE stm32_hal)
target_link_libraries(Platform_STM32F4 PUBLIC pil_adc)
target_link_libraries(Platform_STM32F4 PUBLIC pil_clock)
target_link_libraries(Platform_STM32F4 PUBLIC pil_flash)
target_link_libraries(Platform_STM32F4 PUBLIC pil_gpio)
target_link_libraries(Platform_STM32F4 PUBLIC pil_uart)
target_link_libraries(Platform_STM32F4 PUBLIC pil_watchdog)
