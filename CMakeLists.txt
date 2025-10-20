cmake_minimum_required(VERSION 3.22)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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
# PROJECT
# =========================================================================
project(ha-ctrl LANGUAGES C CXX ASM)

# =========================================================================
# Options to Enable/Disable Parts of Build
# =========================================================================
option(BUILD_APP "Build the main application" ON)
option(BUILD_BOOTLOADER "Build the bootloader" ON)
option(BUILD_TESTING "Build unit tests" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)

# =========================================================================
# Module Path for Custom CMake Files
# =========================================================================
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# =========================================================================
# Output Directory
# =========================================================================
set(CMAKE_BINARY_DIR_BIN ${CMAKE_BINARY_DIR}/../bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR_BIN}) # Ensure the bin directory exists

# =========================================================================
# Toolchain and Build Type Setup
# Firmware Build (Only when NOT testing)
# =========================================================================
if(NOT BUILD_TESTING)
    include("cmake/gcc-arm-none-eabi.cmake")
    include(${CMAKE_SOURCE_DIR}/cmake/utilities.cmake)

endif()

# =========================================================================
# Platform-Specific Settings
# =========================================================================
set(PLATFORM_MCU "STM32F4" CACHE STRING "Target MCU family")
if(PLATFORM_MCU STREQUAL "STM32F4")
    include("${CMAKE_SOURCE_DIR}/cmake/platform-traits.cmake")
elseif(PLATFORM_MCU STREQUAL "NRF52")
    include("${CMAKE_SOURCE_DIR}/cmake/platform-traits-nrf52.cmake")
endif()

add_subdirectory(Platform/${PLATFORM_MCU})

# PIL (platform-independent)
add_subdirectory(Platform/Interface/PilAdc)
add_subdirectory(Platform/Interface/PilUart)
add_subdirectory(Platform/Interface/PilClock)
add_subdirectory(Platform/Interface/PilGpio)
add_subdirectory(Platform/Interface/PilWatchdog)

# =========================================================================
# Add Subdirectories (Targets: Bootloader and App)
# =========================================================================
if(BUILD_APP)
    add_subdirectory(App)
endif()

if(BUILD_BOOTLOADER)
    add_subdirectory(Bootloader)
endif()

# =========================================================================
# Clang-Tidy Support (optional)
# =========================================================================
if(ENABLE_CLANG_TIDY AND NOT BUILD_TESTING)
    include(cmake/clang-tidy.cmake)
endif()

# =========================================================================
# CppUTest Unit Testing Support (host only)
# =========================================================================
if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(extern/cpputest)
    add_subdirectory(tests)
endif()

# =========================================================================
# Build Checks
# =========================================================================
if(NOT BUILD_APP AND NOT BUILD_BOOTLOADER AND NOT BUILD_TESTING)
    message(FATAL_ERROR "All build options (APP, BOOTLOADER, TESTS) are disabled. Nothing to build.")
endif()

if(NOT BUILD_TESTING)
    # ========================================================================
    # Combined Size Check Target (after both bootloader and app are built)
    # ========================================================================
    # ALL → ensures this runs every time you build
    # DEPENDS → ensures ha-ctrl-app and ha-ctrl-boot are built first
    # -Dtarget=... → passes the target name to your script
    # -P → tells CMake to run the script

    add_custom_target(check_size ALL
        COMMAND ${CMAKE_COMMAND}
            -Dtarget=ha-ctrl-app
            -DSCRIPT_OUTPUT_DIR=${CMAKE_BINARY_DIR_BIN}
            -P ${CMAKE_SOURCE_DIR}/cmake/utilities-check-size.cmake
        COMMENT "Running combined size check after build"
        # DEPENDS ha-ctrl-boot ha-ctrl-app # This will ensure boot and app are built first.
    )
    add_dependencies(check_size ha-ctrl-boot ha-ctrl-app)

    # Add combined binary target
    add_combined_binary(ha-ctrl-boot ha-ctrl-app ${PROJECT_NAME}_combined_image)

    add_firmware_packaging(ha-ctrl-app ha-ctrl-boot ${PROJECT_NAME})
endif()

# =========================================================================
# Print Summary of Build Options
# =========================================================================
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_APP:\t\t${BUILD_APP}")
message(STATUS "BUILD_BOOTLOADER:${BUILD_BOOTLOADER}")
message(STATUS "BUILD_TESTING:\t${BUILD_TESTING}")
message(STATUS "CLANG_TIDY:\t\t${ENABLE_CLANG_TIDY}")

# =========================================================================
# Extra Cleaning Target
# =========================================================================
add_custom_target(clean-extra
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_BINARY_DIR_BIN}
        ${CMAKE_BINARY_DIR}/CMakeFiles
        ${CMAKE_SOURCE_DIR}/build-tests
        ${CMAKE_BINARY_DIR}/clang-tidy-warn.log
    COMMENT "Cleaning build artifacts and Clang-Tidy logs."
)
