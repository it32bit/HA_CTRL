cmake_minimum_required(VERSION 3.22)

# =========================================================================
# Project Setup
# =========================================================================
project(ha-ctrl LANGUAGES C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compilation commands into compile_commands.json")

# =========================================================================
# Language Standards
# =========================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================================================================
# Build Options
# =========================================================================
option(BUILD_APP "Build the main application" ON)
option(BUILD_BOOTLOADER "Build the bootloaders: Prim+Sec" ON)
option(BUILD_TESTING "Build unit tests" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)

# =========================================================================
# Paths and Toolchain
# =========================================================================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# =========================================================================
# Output Directory
# =========================================================================
set(CMAKE_BINARY_DIR_BIN ${CMAKE_BINARY_DIR}/../bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR_BIN})

if(NOT BUILD_TESTING)
    include("cmake/gcc-arm-none-eabi.cmake")
    include("cmake/utilities.cmake")
endif()

# =========================================================================
# Platform Traits
# =========================================================================
set(PLATFORM_MCU "STM32F4" CACHE STRING "Target MCU family")
if(PLATFORM_MCU STREQUAL "STM32F4")
    include("cmake/platform-traits.cmake")
elseif(PLATFORM_MCU STREQUAL "NRF52")
    include("cmake/platform-traits-nrf52.cmake")
endif()

# =========================================================================
# Subdirectories
# =========================================================================
add_subdirectory(Platform/${PLATFORM_MCU})
add_subdirectory(Boot)
# PIL (platform-independent)
add_subdirectory(Platform/Interface/PilAdc)
add_subdirectory(Platform/Interface/PilFlash)
add_subdirectory(Platform/Interface/PilUart)
add_subdirectory(Platform/Interface/PilClock)
add_subdirectory(Platform/Interface/PilGpio)
add_subdirectory(Platform/Interface/PilWatchdog)

# =========================================================================
# Subdirectories (Targets: Bootloader's and App)
# =========================================================================
if(BUILD_BOOTLOADER)
    add_subdirectory(BootPrim)
    add_subdirectory(BootSec)
endif()

if(BUILD_APP)
    add_subdirectory(App)
endif()

# =========================================================================
# Clang-Tidy Integration
# =========================================================================
if(ENABLE_CLANG_TIDY AND NOT BUILD_TESTING)
    include("cmake/clang-tidy.cmake")
endif()

# =========================================================================
# Unit Testing
# =========================================================================
if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(extern/cpputest)
    add_subdirectory(tests)
endif()

# =========================================================================
# Build Validation
# =========================================================================
if(NOT BUILD_APP AND NOT BUILD_BOOTLOADER AND NOT BUILD_TESTING)
    message(FATAL_ERROR "All build options (APP, BOOTLOADERS, TESTS) are disabled. Nothing to build.")
endif()

# =========================================================================
# Firmware Build Flow
# =========================================================================
if(NOT BUILD_TESTING)
    # Size Check
    add_custom_target(check_size ALL
        COMMAND ${CMAKE_COMMAND}
            -Dtarget=ha-ctrl-app
            -DSCRIPT_OUTPUT_DIR=${CMAKE_BINARY_DIR_BIN}
            -P ${CMAKE_SOURCE_DIR}/cmake/utilities-check-size.cmake
        COMMENT "Running combined size check after build"
    )
    add_dependencies(check_size ha-ctrl-prim ha-ctrl-sec ha-ctrl-app)



    if(NOT TARGET generate_metadata)
        add_custom_target(generate_metadata ALL
            DEPENDS ${CMAKE_BINARY_DIR_BIN}/app_metadata.bin
                    ${CMAKE_BINARY_DIR_BIN}/bootsec_metadata.bin
        )
    endif()

    # Combined Binary
    add_combined_firmware_with_metadata(
        ha-ctrl-app
        ha-ctrl-sec
        app_metadata.bin
        bootsec_metadata.bin
        ${PROJECT_NAME}_combined_update_image
    )
    add_dependencies(${PROJECT_NAME}_combined_update_image_combined generate_metadata)

    # Firmware Packaging
    add_firmware_packaging(
        ha-ctrl-app
        ha-ctrl-sec
        ha-ctrl-prim
        ${PROJECT_NAME}
    )

    # Extract Flash Address for Combined_Update_Image
    # Read file
    file(READ "${CMAKE_SOURCE_DIR}/Platform/${MCU_FAMILY}/Inc/flash_layout.hpp" FLASH_LAYOUT_CONTENTS)

    # Replace CRLF just in case
    # string(REPLACE "\r" "" FLASH_LAYOUT_CONTENTS "${FLASH_LAYOUT_CONTENTS}")

    # Split into lines
    string(REPLACE "\n" ";" FLASH_LAYOUT_LINES "${FLASH_LAYOUT_CONTENTS}")

    set(NEW_BOOTLOADER2_FLASH_ADDRESS "")

    foreach(line IN LISTS FLASH_LAYOUT_LINES)
        # message(STATUS "Checking line: '${line}'")
        string(REGEX MATCH ".*NEW_BOOTLOADER2_START[ \t]*=[ \t]*0x([0-9A-Fa-f]+)" _match "${line}")
        if(_match)
            message(STATUS "Regex match: '${_match}'")
            message(STATUS "Captured group: '${CMAKE_MATCH_1}'")
            set(NEW_BOOTLOADER2_FLASH_ADDRESS "0x${CMAKE_MATCH_1}")
            break()
        endif()
    endforeach()

    message(STATUS "Combined binary address: ${NEW_BOOTLOADER2_FLASH_ADDRESS}")

    # Flash Target
    add_custom_target(combined-flash
        DEPENDS ${PROJECT_NAME}_combined_update_image_combined
        COMMAND ${STM32CUBE_PROGRAMMER} -c port=SWD -w ${CMAKE_BINARY_DIR_BIN}/${PROJECT_NAME}_combined_update_image.bin 0x08090000 -v -rst
        COMMENT "Flashing combined firmware via ST-Link"
    )
endif()

# =========================================================================
# Build Summary
# =========================================================================
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_APP:\t\t${BUILD_APP}")
message(STATUS "BUILD_BOOTLOADER:${BUILD_BOOTLOADER}")
message(STATUS "BUILD_TESTING:\t${BUILD_TESTING}")
message(STATUS "CLANG_TIDY:\t\t${ENABLE_CLANG_TIDY}")

# =========================================================================
# Cleanup Target
# =========================================================================
add_custom_target(clean-extra
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_BINARY_DIR_BIN}
        ${CMAKE_BINARY_DIR}/CMakeFiles
        ${CMAKE_SOURCE_DIR}/build-tests
        ${CMAKE_BINARY_DIR}/clang-tidy-warn.log
    COMMENT "Cleaning build artifacts and Clang-Tidy logs."
)
