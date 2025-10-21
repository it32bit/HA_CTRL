# =========================================================================
# Project Sources and Configuration
# =========================================================================
include("${CMAKE_SOURCE_DIR}/cmake/platform-traits.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/platform-traits.cmake")

# CPU and FPU configuration for STM32F407
set(cpu_PARAMS
    -mthumb
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# Source files used by App and Bootloader
set(sources_SRCS
    ${CMAKE_SOURCE_DIR}/Core/Src/syscall.c
    ${CMAKE_SOURCE_DIR}/Core/Src/sysmem.c
    ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32f4xx.c
    ${CMAKE_SOURCE_DIR}/Core/Src/stm32f4xx_it.c

    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal.c
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_rcc.c
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_gpio.c
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_cortex.c
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_rcc_ex.c
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_ll_exti.c
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_ll_rcc.c
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_ll_utils.c

    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/gpio_manager_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/gpio_pin_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/gpio_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/watchdog_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/watchdog_manager_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/clock_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/clock_manager_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/adc_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/adc_manager_stm32.cpp
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Src/flash_writer_stm32.cpp

    ${CMAKE_SOURCE_DIR}/Startup/startup_stm32f407vgtx.s
    ${CMAKE_SOURCE_DIR}/App/Src/app.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/app_it.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/console.cpp
    ${CMAKE_SOURCE_DIR}/App/Src/uart_redirect.cpp

    ${CMAKE_SOURCE_DIR}/Boot/Src/boot.cpp
    ${CMAKE_SOURCE_DIR}/BootPrim/Src/boot_prim.cpp
    ${CMAKE_SOURCE_DIR}/BootSec/Src/boot_sec.cpp
)

# Include directories
set(include_HEADERS_DIRS
    ${CMAKE_SOURCE_DIR}/App/Inc
    ${CMAKE_SOURCE_DIR}/Boot/Inc
    ${CMAKE_SOURCE_DIR}/BootPrim/Inc
    ${CMAKE_SOURCE_DIR}/BootSec/Inc
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Platform/Interface
    ${CMAKE_SOURCE_DIR}/Platform/${PLATFORM_MCU}/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/cmsis-device-f4/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
)

# Compiler and linker options (can be extended per target)
set(compiler_OPTS ${compiler_OPTS})
set(linker_OPTS ${linker_OPTS})
