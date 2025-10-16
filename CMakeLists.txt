cmake_minimum_required(VERSION 3.22)
project(ha-ctrl LANGUAGES C CXX ASM)

# =========================================================================
# Options to Enable/Disable Parts of the Build
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
# Language Standards
# =========================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =========================================================================
# Toolchain and Build Type Setup
# Firmware Build (Only when NOT testing)
# =========================================================================
if(NOT BUILD_TESTING)
    include("cmake/gcc-arm-none-eabi.cmake")

    # Load auto-generated sources, includes, and flags
    # vscode_generated.cmake" included in subdirectories boot and app

    # Remove default main.c (don't build it)
    list(REMOVE_ITEM sources_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/main.c")
endif()

message("Build type: ${CMAKE_BUILD_TYPE}")

# =========================================================================
# Output Directory
# =========================================================================
set(CMAKE_BINARY_DIR_BIN ${CMAKE_BINARY_DIR}/../bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR_BIN}) # Ensure the bin directory exists

# =========================================================================
# Platform-Specific Settings
# =========================================================================
add_subdirectory(Platform/STM32F4)

# =========================================================================
# Add Subdirectories (Targets: Bootloader and App)
# =========================================================================
# NOTE: Pass sources_SRCS variable to subdirectories expicitly if needed.
if(BUILD_APP)
    add_subdirectory(App)
endif()

if(BUILD_BOOTLOADER)
    add_subdirectory(Bootloader)
endif()


# =========================================================================
# Extra Cleaning Target
# =========================================================================
add_custom_target(clean-extra
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_BINARY_DIR_BIN}/bin/*
        ${CMAKE_BINARY_DIR}/CMakeFiles/*
        ${CMAKE_SOURCE_DIR}/build-tests/*
    COMMENT "Cleaning build products (*.elf, *.map, *.hex, *.bin)"
)

# =========================================================================
# Clang-Tidy Support (optional)
# =========================================================================
if(ENABLE_CLANG_TIDY AND NOT BUILD_TESTING)
    # Keep your full clang-tidy logic here (unchanged)...
    # [omitted for brevity — include everything from your script if needed]

    set(include_clang_tidy ${include_clang_tidy}
        ${include_HEADERS_DIRS}
        /opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/lib/gcc/arm-none-eabi/13.3.1/include
        /opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/arm-none-eabi/include/c++/13.3.1
    )

    if(ENABLE_CLANG_TIDY)
        # Find the clang-tidy executable
        find_program(CLANG_TIDY_EXE NAMES clang-tidy)

        if(CLANG_TIDY_EXE)
            message(STATUS "Clang-Tidy found: ${CLANG_TIDY_EXE}")

            # Define the output log file path in the build directory root.
            set(CLANG_TIDY_LOG_FILE "${CMAKE_BINARY_DIR}/clang-tidy-warn.log")

            # Define optional extra arguments for clang-tidy
            # Example: set(CLANG_TIDY_EXTRA_ARGS "-checks=bugprone-*,readability-*")
            #set(CLANG_TIDY_EXTRA_ARGS "")
            # Combine ALL arguments (ADD the new -I flag for project headers)
            set(CLANG_TIDY_EXTRA_ARGS
                "${ARM_TARGET_TRIPLE} --extra-arg=-I${include_clang_tidy} --extra-arg=-isystem${ARM_CXX_INCLUDE_PATH} --extra-arg=-isystem${ARM_CXX_TARGET_INCLUDE_PATH} --extra-arg=-isystem${ARM_BASE_INCLUDE_PATH} --extra-arg=-isystem${ARM_GCC_INTERNAL_INCLUDE_PATH}"
            )

            # 1. Collect only the source files you want to check (adjust GLOB_RECURSE as needed)
            # Use full paths to avoid issues with the custom target's working directory.
            file(GLOB_RECURSE APP_SOURCE_FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/App/Src/*.c"
                "${CMAKE_CURRENT_SOURCE_DIR}/App/Src/*.cpp"
            )

            # Join the list of files into a space-separated string for the bash loop
            list(JOIN APP_SOURCE_FILES " " APP_FILES_SPACE_SEPARATED)

            # Use canonical (simplified) paths for better reliability with Clang
            # NOTE: Removed all "bin/../lib/gcc/../.." parts.
            set(TOOLCHAIN_BASE_PATH "/opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32")

            # 1. C++ Standard Library path (for files like 'list')
            set(ARM_CXX_INCLUDE_PATH
                "${TOOLCHAIN_BASE_PATH}/arm-none-eabi/include/c++/13.3.1"
            )

            # 2. Base System Include path (for the final required C++ internal headers)
            # NEW CRUCIAL PATH: C++ Target-specific internal headers
            set(ARM_CXX_TARGET_INCLUDE_PATH
                "${TOOLCHAIN_BASE_PATH}/arm-none-eabi/include/c++/13.3.1/arm-none-eabi"
            )

            # 3. Base System Include path (for 'bits/c++config.h' dependencies)
            set(ARM_BASE_INCLUDE_PATH
                "${TOOLCHAIN_BASE_PATH}/arm-none-eabi/include"
            )

            # 4. GCC Internal Include path (contains critical 'bits' files)
            set(ARM_GCC_INTERNAL_INCLUDE_PATH
                "${TOOLCHAIN_BASE_PATH}/lib/gcc/arm-none-eabi/13.3.1/include"
            )

            # Define the target triple for the cross-compiler (FIXED TYPO)
            set(ARM_TARGET_TRIPLE "--extra-arg=--target=arm-none-eabi")

            # FIX: Force the arguments into a single, space-separated string literal.
            # NOTE: This line MUST NOT contain semicolons or be broken into multiple lines with comments/newlines inside the quotes.
            # Combine ALL FOUR arguments (C++ Target-specific path added)
            set(CLANG_TIDY_EXTRA_ARGS
                "${ARM_TARGET_TRIPLE} --extra-arg=-isystem${ARM_CXX_INCLUDE_PATH} --extra-arg=-isystem${ARM_CXX_TARGET_INCLUDE_PATH} --extra-arg=-isystem${ARM_BASE_INCLUDE_PATH} --extra-arg=-isystem${ARM_GCC_INTERNAL_INCLUDE_PATH}"
            )

            # Redefine the variables needed for the command string
            set(CLANG_TIDY_EXE_LITERAL "${CLANG_TIDY_EXE}")
            set(CLANG_TIDY_ARGS_LITERAL "-p \"${CMAKE_BINARY_DIR}\" ${CLANG_TIDY_EXTRA_ARGS}")
            set(LOG_PATH_LITERAL "${CLANG_TIDY_LOG_FILE}")

            # Use the variables to construct ONE single-line literal string.
            # Ensure this is all on one line in your file!
            set(CLANG_TIDY_FULL_COMMAND
            "echo \"Running Clang-Tidy on ${APP_FILES_SPACE_SEPARATED} source(s). Output in ${LOG_PATH_LITERAL}\" > \"${LOG_PATH_LITERAL}\";echo \"File processing started (check log for details)\" >> \"${LOG_PATH_LITERAL}\";for file in ${APP_FILES_SPACE_SEPARATED}; do printf \"\\n--- Running Clang-Tidy on %s ---\\n\" \"\$file\" >> \"${LOG_PATH_LITERAL}\";exec ${CLANG_TIDY_EXE_LITERAL} ${CLANG_TIDY_ARGS_LITERAL} \"\$file\" >> \"${LOG_PATH_LITERAL}\" 2>&1 || { printf \"Clang-Tidy failed on %s. See log for details.\\n\" \"\$file\" ; exit 1 ; } ; done"
            )

            # You can remove the message line to reduce clutter, but if you keep it, it should now show a perfectly clean string.
            # message(STATUS "DEBUG COMMAND: ${CLANG_TIDY_FULL_COMMAND}")

            # Ensure the custom target uses this clean variable.
            add_custom_target(clang_tidy_log
                COMMAND bash -c "${CLANG_TIDY_FULL_COMMAND}"
                VERBATIM
                COMMENT "Logging Clang-Tidy output to ${CLANG_TIDY_LOG_FILE}"
                USES_TERMINAL
            )

            set(CLANG_TIDY_FULL_COMMAND_CONSOLE
            "echo \"Running Clang-Tidy on ${APP_FILES_SPACE_SEPARATED} source(s). Output is directed to terminal (stdout).\" ;echo \"File processing started.\" ;for file in ${APP_FILES_SPACE_SEPARATED}; do printf \"\\n--- Running Clang-Tidy on %s ---\\n\" \"\$file\" ;${CLANG_TIDY_EXE_LITERAL} ${CLANG_TIDY_ARGS_LITERAL} \"\$file\" 2>&1 || { printf \"Clang-Tidy failed on %s. See terminal output for details.\\n\" \"\$file\" ; exit 1 ; } ; done"
            )

            # Ensure the custom target uses this clean variable.
            add_custom_target(clang_tidy_console
                COMMAND bash -c "${CLANG_TIDY_FULL_COMMAND_CONSOLE}"
                VERBATIM
                COMMENT "Logging Clang-Tidy output to ${CLANG_TIDY_LOG_FILE}"
                USES_TERMINAL
            )

            set(CLANG_TIDY_FULL_COMMAND_TEE "echo \"Running Clang-Tidy on ${APP_FILES_SPACE_SEPARATED} source(s). Output being mirrored to ${LOG_PATH_LITERAL}\" | tee \"${LOG_PATH_LITERAL}\";echo \"File processing started (check log for details)\" | tee -a \"${LOG_PATH_LITERAL}\";for file in ${APP_FILES_SPACE_SEPARATED}; do printf \"\\n--- Running Clang-Tidy on %s ---\\n\" \"\$file\" | tee -a \"${LOG_PATH_LITERAL}\";${CLANG_TIDY_EXE_LITERAL} ${CLANG_TIDY_ARGS_LITERAL} \"\$file\" 2>&1 | tee -a \"${LOG_PATH_LITERAL}\" || { printf \"Clang-Tidy failed on %s. See log for details.\\n\" \"\$file\" ; exit 1 ; } ; done")

            # Ensure the custom target uses this clean variable.
            add_custom_target(clang_tidy_both
                COMMAND bash -c "${CLANG_TIDY_FULL_COMMAND_TEE}"
                VERBATIM
                COMMENT "Logging Clang-Tidy output to ${CLANG_TIDY_LOG_FILE}"
                USES_TERMINAL
            )

        else()
            message(WARNING "Clang-Tidy not found! Skipping static analysis target.")
        endif()
    endif()
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
# Print Summary of Build Options
# =========================================================================
message(STATUS "BUILD_APP: ${BUILD_APP}")
message(STATUS "BUILD_BOOTLOADER: ${BUILD_BOOTLOADER}")
message(STATUS "BUILD_TESTING: ${BUILD_TESTING}")
message(STATUS "ENABLE_CLANG_TIDY: ${ENABLE_CLANG_TIDY}")

if(NOT BUILD_APP AND NOT BUILD_BOOTLOADER AND NOT BUILD_TESTS)
    message(FATAL_ERROR "All build options (APP, BOOTLOADER, TESTS) are disabled. Nothing to build.")
endif()

# ========================================================================
# Combined Size Check Target (after both bootloader and app are built)
# ========================================================================
# ALL → ensures this runs every time you build
# DEPENDS → ensures ha-ctrl-app and ha-ctrl-boot are built first
# -Dtarget=... → passes the target name to your script
# -P → tells CMake to run the script

add_custom_target(check_size ALL
    COMMAND ${CMAKE_COMMAND} -Dtarget=ha-ctrl-app -P ${CMAKE_SOURCE_DIR}/cmake/utilities-check-size.cmake
    COMMENT "Running combined size check after build"
    DEPENDS ha-ctrl-boot ha-ctrl-app
    USES_TERMINAL
)

# =========================================================================
# Additional notes and setup for sources_SRCS variable
# =========================================================================
# To pass the sources_SRCS from vscode_generated.cmake into subdirectories
# (App, Bootloader), just rely on the include of vscode_generated.cmake
# in App/CMakeLists.txt and Bootloader/CMakeLists.txt files.:
#    include("${CMAKE_SOURCE_DIR}/cmake/vscode_generated.cmake")

