cmake_minimum_required(VERSION 3.22)

project(ha-ctrl)
enable_language(C CXX ASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# =========================================================================
# Build Type & Toolchain
# =========================================================================
if(NOT BUILD_TESTING)
    message("Build CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
    include("cmake/gcc-arm-none-eabi.cmake")
    message("Build CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})
endif()

message("Build type: " ${CMAKE_BUILD_TYPE})

# =========================================================================
# Core Compiler Settings
# =========================================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# =========================================================================
# Optionally build firmware (only when NOT building tests)
# =========================================================================
if(NOT BUILD_TESTING)
    # Core MCU flags, CPU type, instruction set and FPU setup
    set(cpu_PARAMS)

    # Sources
    set(sources_SRCS)

    # Include directories
    set(include_DIRS)
    set(include_c_DIRS)
    set(include_cxx_DIRS)
    set(include_asm_DIRS)

    # Symbols
    set(symbols_SYMB)
    set(symbols_c_SYMB)
    set(symbols_cxx_SYMB)
    set(symbols_asm_SYMB)

    # Linking
    set(link_DIRS)
    set(link_LIBS)
    set(linker_script_SRC)

    # Compiler/Linker flags
    set(compiler_OPTS)
    set(linker_OPTS)

    # Output binary location
    set(CMAKE_BINARY_DIR_BIN ${CMAKE_CURRENT_SOURCE_DIR}/bin)

    include("cmake/vscode_generated.cmake")

    link_directories(${CMAKE_PROJECT_NAME} ${link_DIRS})

    add_executable(${CMAKE_PROJECT_NAME})

    target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${sources_SRCS})

    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${include_DIRS}
        $<$<COMPILE_LANGUAGE:C>: ${include_c_DIRS}>
        $<$<COMPILE_LANGUAGE:CXX>: ${include_cxx_DIRS}>
        $<$<COMPILE_LANGUAGE:ASM>: ${include_asm_DIRS}>
    )

    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        ${symbols_SYMB}
        $<$<COMPILE_LANGUAGE:C>: ${symbols_c_SYMB}>
        $<$<COMPILE_LANGUAGE:CXX>: ${symbols_cxx_SYMB}>
        $<$<COMPILE_LANGUAGE:ASM>: ${symbols_asm_SYMB}>
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>: >
        STM32F407xx
        USE_HAL_DRIVER
        USE_FULL_LL_DRIVER
    )

    target_link_libraries(${CMAKE_PROJECT_NAME} ${link_LIBS})

    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        ${cpu_PARAMS}
        ${compiler_OPTS}
        -Wall -Wextra -Wpedantic -Wno-unused-parameter
        $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -MMD -MP>
        $<$<CONFIG:Debug>:-Og -g3 -ggdb>
        $<$<CONFIG:Release>:-Og -g0>
    )

    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR_BIN}
    )

    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        -T${linker_script_SRC}
        ${cpu_PARAMS}
        ${linker_OPTS}
        -Wl,-Map=${CMAKE_BINARY_DIR_BIN}/${CMAKE_PROJECT_NAME}.map
        -u _printf_float
        --specs=nosys.specs
        -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
        -Wl,-z,max-page-size=8
        -Wl,--print-memory-usage
    )

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_BINARY_DIR_BIN}/${CMAKE_PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${CMAKE_BINARY_DIR_BIN}/${CMAKE_PROJECT_NAME}.bin
        COMMENT "Generating HEX and BIN file from ELF"
    )

    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "******** Print size information:"
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        COMMENT "Size information"
    )

# Flash target using STM32CubeProgrammer CLI
    add_custom_target(flash
        DEPENDS ${CMAKE_PROJECT_NAME}
        COMMAND ${STM32CUBE_PROGRAMMER} -c port=SWD -d ${CMAKE_BINARY_DIR_BIN}/${CMAKE_PROJECT_NAME}.elf
        COMMENT "Flashing firmware to STM32 via ST-Link"
    )

endif()

# Extra clean target to remove generated binary files
add_custom_target(clean-extra
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        ${CMAKE_SOURCE_DIR}/bin/*
        ${CMAKE_BINARY_DIR}/CMakeFiles/${CMAKE_PROJECT_NAME}.dir/*
        ${CMAKE_SOURCE_DIR}/build-tests/*
    COMMENT "Removing generated binary files (*.elf, *.map, *.hex, *.bin)"
)

# =========================================================================
# Clang-Tidy (only runs when building firmware)
# =========================================================================
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)

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
# CppUTest Unit Test Support (host only)
# =========================================================================
option(BUILD_TESTING "Build unit tests" ON)

if(BUILD_TESTING AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(extern/cpputest)
    add_subdirectory(tests)
endif()
