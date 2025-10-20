# =========================================================================
# include(cmake/platform-traits.cmake)
# =========================================================================
# Platform Traits Configuration
# =========================================================================
# This file defines platform-specific traits for memory limits, MCU identity,
# and optional build flags. It can be extended to support multiple MCU families.
# =========================================================================
# STM32F407VG has: (1048576) 1 MB FLASH, (196608) 192 KB RAM
# Limits is set to 90% of total to leave some headroom:
# - FLASH: 917504 bytes (896 KB)
# - RAM:   131072 bytes (128 KB)
# This allows for a maximum usage of:
#     FLASH: 1 MB (1024 KB)
#     RAM: 192 KB (split between SRAM and CCMRAM)
# =========================================================================
# MCU identity
set(MCU_FAMILY "STM32F4")
set(MCU_MODEL "STM32F407VGTX")
set(MCU_FLASH_TOTAL 1048576)
set(MCU_RAM_TOTAL   196608)

math(EXPR MCU_FLASH_LIMIT "${MCU_FLASH_TOTAL} * 90 / 100")  # 917504
math(EXPR MCU_RAM_LIMIT   "${MCU_RAM_TOTAL} * 90 / 100")     # 131072

# Compiler defines
set(MCU_DEFINES
    STM32F407xx
    USE_HAL_DRIVER
    USE_FULL_LL_DRIVER
)

# Platform-specific include directories
set(MCU_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/Platform/${MCU_FAMILY}/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/cmsis-device-f4/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
)

