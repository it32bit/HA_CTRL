# =========================================================================
# Clang-Tidy Support (optional)
# =========================================================================

if(ENABLE_CLANG_TIDY AND NOT BUILD_TESTING)

    # Use canonical (simplified) paths for better reliability with Clang
    # NOTE: Removed all "bin/../lib/gcc/../.." parts.

    # Toolchain base path
    set(TOOLCHAIN_BASE_PATH "/opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32")

    # 1. C++ Standard Library path (for files like 'list')
    # 2. Base System Include path (for the final required C++ internal headers) NEW CRUCIAL PATH: C++ Target-specific internal headers
    # 3. Base System Include path (for 'bits/c++config.h' dependencies)
    # 4. GCC Internal Include path (contains critical 'bits' files)
    # Include paths for Clang-Tidy
    set(ARM_CXX_INCLUDE_PATH "${TOOLCHAIN_BASE_PATH}/arm-none-eabi/include/c++/13.3.1")
    set(ARM_CXX_TARGET_INCLUDE_PATH "${TOOLCHAIN_BASE_PATH}/arm-none-eabi/include/c++/13.3.1/arm-none-eabi")
    set(ARM_BASE_INCLUDE_PATH "${TOOLCHAIN_BASE_PATH}/arm-none-eabi/include")
    set(ARM_GCC_INTERNAL_INCLUDE_PATH "${TOOLCHAIN_BASE_PATH}/lib/gcc/arm-none-eabi/13.3.1/include")

    # Define the target triple for the cross-compiler (FIXED TYPO)
    set(ARM_TARGET_TRIPLE "--extra-arg=--target=arm-none-eabi")


    # FIX: Force the arguments into a single, space-separated string literal.
    # NOTE: This line MUST NOT contain semicolons or be broken into multiple lines with comments/newlines inside the quotes.
    # Combine ALL FOUR arguments (C++ Target-specific path added)
    set(CLANG_TIDY_EXTRA_ARGS
        "${ARM_TARGET_TRIPLE} --extra-arg=-isystem${ARM_CXX_INCLUDE_PATH} --extra-arg=-isystem${ARM_CXX_TARGET_INCLUDE_PATH} --extra-arg=-isystem${ARM_BASE_INCLUDE_PATH} --extra-arg=-isystem${ARM_GCC_INTERNAL_INCLUDE_PATH}"
    )

    # Find clang-tidy
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(NOT CLANG_TIDY_EXE)
        message(WARNING "Clang-Tidy requested but not found")
        return()
    endif()

    message(STATUS "Clang-Tidy found: ${CLANG_TIDY_EXE}")



    # 1. Collect only the source files you want to check (adjust GLOB_RECURSE as needed)
    # Use full paths to avoid issues with the custom target's working directory.
    file(GLOB_RECURSE APP_SOURCE_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/App/Src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/App/Src/*.cpp"
    )

    # Join the list of files into a space-separated string for the bash loop
    list(JOIN APP_SOURCE_FILES " " APP_FILES_SPACE_SEPARATED)

    # Log file path in the build directory root.
    set(CLANG_TIDY_LOG_FILE "${CMAKE_BINARY_DIR}/clang-tidy-warn.log")

    # Literal command components
    set(CLANG_TIDY_EXE_LITERAL "${CLANG_TIDY_EXE}")
    set(CLANG_TIDY_ARGS_LITERAL "-p \"${CMAKE_BINARY_DIR}\" ${CLANG_TIDY_EXTRA_ARGS}")
    set(LOG_PATH_LITERAL "${CLANG_TIDY_LOG_FILE}")

    # Use the variables to construct ONE single-line literal string.
    # Ensure this is all on one line in your file!
    set(CLANG_TIDY_FULL_COMMAND
        "echo \"Running Clang-Tidy on ${APP_FILES_SPACE_SEPARATED} source(s). Output in ${LOG_PATH_LITERAL}\" > \"${LOG_PATH_LITERAL}\";echo \"File processing started (check log for details)\" >> \"${LOG_PATH_LITERAL}\";for file in ${APP_FILES_SPACE_SEPARATED}; do printf \"\\n--- Running Clang-Tidy on %s ---\\n\" \"\$file\" >> \"${LOG_PATH_LITERAL}\";exec ${CLANG_TIDY_EXE_LITERAL} ${CLANG_TIDY_ARGS_LITERAL} \"\$file\" >> \"${LOG_PATH_LITERAL}\" 2>&1 || { printf \"Clang-Tidy failed on %s. See log for details.\\n\" \"\$file\" ; exit 1 ; } ; done"
    )

    # [OPTIONAL]The message should now show a perfectly clean string.
    # message(STATUS "DEBUG COMMAND: ${CLANG_TIDY_FULL_COMMAND}")

    # Ensure the custom target uses this clean variable.
    add_custom_target(clang_tidy_log
        COMMAND bash -c "${CLANG_TIDY_FULL_COMMAND}"
        VERBATIM
        COMMENT "Logging Clang-Tidy output to ${CLANG_TIDY_LOG_FILE}"
        USES_TERMINAL
    )

    # Mode: console only
    set(CLANG_TIDY_FULL_COMMAND_CONSOLE
        "echo \"Running Clang-Tidy on ${APP_FILES_SPACE_SEPARATED} source(s). Output is directed to terminal.\" ;echo \"File processing started.\" ;for file in ${APP_FILES_SPACE_SEPARATED}; do printf \"\\n--- Running Clang-Tidy on %s ---\\n\" \"\$file\" ;${CLANG_TIDY_EXE_LITERAL} ${CLANG_TIDY_ARGS_LITERAL} \"\$file\" 2>&1 || { printf \"Clang-Tidy failed on %s.\\n\" \"\$file\" ; exit 1 ; } ; done"
    )

    # Ensure the custom target uses this clean variable.
    add_custom_target(clang_tidy_console
        COMMAND bash -c "${CLANG_TIDY_FULL_COMMAND_CONSOLE}"
        VERBATIM
        COMMENT "Logging Clang-Tidy output to ${CLANG_TIDY_LOG_FILE}"
        USES_TERMINAL
    )

    # Mode: both log and console
    set(CLANG_TIDY_FULL_COMMAND_TEE
        "echo \"Running Clang-Tidy on ${APP_FILES_SPACE_SEPARATED} source(s). Output mirrored to ${LOG_PATH_LITERAL}\" | tee \"${LOG_PATH_LITERAL}\";echo \"File processing started\" | tee -a \"${LOG_PATH_LITERAL}\";for file in ${APP_FILES_SPACE_SEPARATED}; do printf \"\\n--- Running Clang-Tidy on %s ---\\n\" \"\$file\" | tee -a \"${LOG_PATH_LITERAL}\";${CLANG_TIDY_EXE_LITERAL} ${CLANG_TIDY_ARGS_LITERAL} \"\$file\" 2>&1 | tee -a \"${LOG_PATH_LITERAL}\" || { printf \"Clang-Tidy failed on %s.\\n\" \"\$file\" ; exit 1 ; } ; done"
    )

    # Ensure the custom target uses this clean variable.
    add_custom_target(clang_tidy_both
        COMMAND bash -c "${CLANG_TIDY_FULL_COMMAND_TEE}"
        VERBATIM
        COMMENT "Logging Clang-Tidy output to ${CLANG_TIDY_LOG_FILE}"
        USES_TERMINAL
    )
endif()
