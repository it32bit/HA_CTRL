# Platform/STM32F4/CMakeLists.txt
# App target
set(PLATFORM_TARGET Platform_STM32F4)



add_library(${PLATFORM_TARGET} STATIC
    Src/gpio_manager_stm32.cpp
    Src/gpio_pin_stm32.cpp
    Src/gpio_hal_stm32.cpp
    Src/watchdog_stm32.cpp
    # Add other .cpp files
    Src/hal_uart.cpp
    Src/hal_adc.cpp

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
    STM32F407xx
    USE_HAL_DRIVER
    USE_FULL_LL_DRIVER
)
# TODO: remove USE_HAL_DRIVER when HAL is fully gone

# Link HAL/LL libraries here if needed
# target_link_libraries(${PLATFORM_TARGET} PRIVATE stm32_hal)
target_link_libraries(Platform_STM32F4 PUBLIC pil_gpio)
target_link_libraries(Platform_STM32F4 PUBLIC pil_watchdog)

