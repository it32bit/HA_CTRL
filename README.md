# CPP_HA_CTRL

System Supporting Smart Home Control

## Host

Ubuntu 24.04 Lts
VSCode

## CPU

STM32F407G

## Board

STM32F407G-DISC1

## Submodules

git submodule add <https://github.com/STMicroelectronics/stm32f4xx-hal-driver.git> Drivers/stm32f4xx-hal-driver
git submodule add <https://github.com/STMicroelectronics/cmsis-device-f4.git> Drivers/cmsis-device-f4

## Linux usefull command

    sudo ln -s /opt/stm32cubeclt_1.19.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI /usr/bin/

## VSCode

    VS Code can hide folders based on your settings.
    Fix:
        Open Command Palette (Ctrl+Shift+P) → type Preferences: Open Settings (JSON) and check for missing folders.

    Common issue in VS Code when IntelliSense doesn't pick up preprocessor definitions that are correctly passed to the compiler via CMake.
    Sections like #ifdef MY_DEFINICTION are properly recognized and not shadowed:

    Fix: IntelliSense Preprocessor Defines in VS Code:
        1.Enable CMake Configuration Provider In your .vscode/settings.json, add:
        {
        "CMake.configureOnOpen": true,
        "CMake: UseCMakePresets": "always"
        }
        2.Make sure your CMake build generates a compile_commands.json file:
            set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
        3.Point VS Code to compile_commands.json in .vscode/c_cpp_properties.json, set:
            "configurations": [
                {
                ...
                "defines": [
                    "HAL_LIB"
                ],
                "compileCommands": "${workspaceFolder}/build/debug/compile_commands.json"
                }
            ]

## Problems

Solution for incorrect toolchain path:

    [Fille:cmake/gcc-arm-none-eabi.cmake]
    set(TOOLCHAIN_PATH "/opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin")

Solution for TAG: "READONLY" in *_FLASH.ld  file. The "READONLY" keyword is only supported in GCC11 and later,
remove it (in whole file) if using GCC10 or earlier.

    [Fille:stm32f407vgtx_FLASH.ld]
    ...
    .init_array (READONLY) :
    ...

Solution for missing path headers from stm3244xx_hal.h in *.{cpp, hpp} files
    Section include_c_DIRS include_cxx_DIRS and should contain the same folders
    # Include directories
    set(include_c_DIRS ${include_c_DIRS}

        ${CMAKE_CURRENT_SOURCE_DIR}/App/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/cmsis-device-f4/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Core/Include
    )
    set(include_cxx_DIRS ${include_cxx_DIRS}

        ${CMAKE_CURRENT_SOURCE_DIR}/App/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/cmsis-device-f4/Include
        ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Core/Include
    )

Solution for error: unknown type name 'class' in ha_ctrl.hpp
    Move your class LedController to a separate led_ctrl.hpp file and expose only C-compatible functions in the header.

## VS Code Settings

"C_Cpp.errorSquiggles": "enabled" - will show error squiggles for: Missing includes, Type mismatches, Undefined symbols, Other IntelliSense-based diagnostics

## GNU Tools for STM32

"$ /opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc --version"

arm-none-eabi-gcc (GNU Tools for STM32 13.3.rel1.20240926-1715) 13.3.1 20240614
Copyright (C) 2023 Free Software Foundation, Inc.
