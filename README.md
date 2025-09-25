# Home Automation Control Unit – Embedded Project

This project is a home automation control unit built around the **STM32F4-DISC1** development board. It is designed to manage key household systems including:

- Lighting
- Gate control
- Air conditioning

## Software Stack

- Embedded Platform: `STM32F4-DISC1`
- Microcontroller: `STM32F407G`
- Programming Language: `C++`
- Host Environment: `Ubuntu 24.04 LTS`
- IDE: `VSCode` with extensions and `STM32CubeMX`
- Toolchain: `GCC`, `VSCode`, `CMake`
- Code Formatting: `clang-format`

## GNU Tools for STM32

```C
cmake --version
cmake version 3.28.3
CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

```C
arm-none-eabi-gcc --version
arm-none-eabi-gcc (GNU Tools for STM32 13.3.rel1.20240926-1715) 13.3.1 20240614
Copyright (C) 2023 Free Software Foundation, Inc.
```

```C
ninja --version
1.11.1
```

## VSCode Extensions

```json
{
    "recommendations": [
        "ms-vscode.cpptools",                   // (dependencies to ms-vscode.cpptools-extension-pack)
        "ms-vscode.cpptools-themes",            // (dependencies to ms-vscode.cpptools-extension-pack)
        "ms-vscode.cmake-tools",                // (dependencies to ms-vscode.cpptools-extension-pack)
        "ms-vscode.cpptools-extension-pack",    // Provides CMake and C++ file coloring, completion & support
        "twxs.cmake",                           // (dependencies to ms-vscode.cpptools-extension-pack)
        "dan-c-underwood.arm",                  // Provides syntax highlighting for the Arm Assembly language
        "zixuanwang.linkerscript",              // Provides syntax highlighting for linker scripts
        "ms-vscode.hexeditor",                  // Provides hex editor fo viewing & anipulating files in their raw hexadecimal representation
        "trond-snekvik.gnu-mapfiles",           // Provides syntax highlighting and symbol listing for GNU linker .map files
        "jeff-hykin.better-cpp-syntax",         // Provides syntax highlighting for C++
        "marus25.cortex-debug",                 // Provides debug support on Arm Cortex-M
        "mcu-debug.debug-tracker-vscode",       // Dependencies to "marus25.cortex-debug"
        "mcu-debug.memory-view",                // Dependencies to "marus25.cortex-debug"
        "mcu-debug.peripheral-viewer",          // Dependencies to "marus25.cortex-debug"
        "mcu-debug.rtos-views"                  // Dependencies to "marus25.cortex-debug"
    ]
}
```

## Git Submodules

The project uses the following external dependencies:

``` bash
[submodule "Drivers/stm32f4xx-hal-driver"]
    path = Drivers/stm32f4xx-hal-driver
    url = https://github.com/STMicroelectronics/stm32f4xx-hal-driver.git
[submodule "Drivers/cmsis-device-f4"]
    path = Drivers/cmsis-device-f4
    url = https://github.com/STMicroelectronics/cmsis-device-f4.git
```

## Clock configuration

<img src="./Doc/HaCtrl-ClockConfig.png" alt="Clock configuration">

## Project Status

The board is currently in its initial state.
At this stage, only basic LED blinking functionality has been implemented. No control logic or peripheral interaction is in place yet.

## Goals

- Develop a robust and modular control application
- Ensure reliable communication between host and embedded system
- Transition to a custom HAL for optimized performance and flexibility

*Stay tuned for updates as the project evolves!*
