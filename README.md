# Home Automation Control Unit – Embedded Project

This project is a home automation control unit built around the **STM32F4-DISC1** development board. It is designed to manage key household systems including:

- Lighting
- Gate control
- Air conditioning

## Project Status

### Bootloader Overview

The bootloader is responsible for secure startup, firmware validation, and controlled firmware updates. It supports both ST-Link and UART (serial) interfaces for loading the combined firmware image.

#### Firmware Integrity Check

Verifies the integrity of the active application using a CRC32 checksum before executing it.

#### New Firmware Validation

Confirms that any new firmware image written to the update region is valid and complete (metadata, certificate, and image CRC verification).

#### Firmware Update Process

Handles in-field updates using a combined image that includes:

- Secondary bootloader
- Application binary
- Corresponding metadata and certificates for both regions

After validation, the bootloader copies the new images into their active regions safely.

#### Dual-Boot Architecture

Utilizes two separate firmware regions — active and update — to ensure rollback safety and prevent bricking during failed updates.

#### Communication Interfaces

- ST-Link: Used for direct programming via SWD during development or recovery.
- Serial UART: Used for remote updates; supports reception of the combined image and triggers the update sequence automatically.

#### Metadata Handling

Each image (bootloader and application) includes a small metadata block containing:

- CRC32 checksum
- Firmware size
- Version information

#### Certificates and Security Placeholders

Each image region reserves 512B for certificate data (future-proofing for digital signature validation).

### Application

- At this stage, only basic LED blinking functionality has been implemented. No control logic or peripheral interaction is in place yet.

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
[submodule "extern/cpputest"]
    path = extern/cpputest
    url = https://github.com/cpputest/cpputest.git
```

## Clock configuration

<!-- <img src="./Doc/HaCtrl-ClockConfig.png" alt="Clock configuration"> -->

| Parameter                  | Value        | Notes                            |
|---------------------------|--------------|----------------------------------|
| System Clock Source       | PLL (HSE)    | Phase-locked loop using HSE      |
| SYSCLK                    | 168 MHz      | System clock                     |
| HCLK                      | 168 MHz      | AHB bus clock                    |
| AHB Prescaler             | 1            | No division                      |
| APB1 Prescaler            | 4            | APB1 clock = 42 MHz              |
| APB2 Prescaler            | 2            | APB2 clock = 84 MHz              |
| HSE Frequency             | 8 MHz        | External crystal frequency       |
| PLL_M                     | 8            | Input divider                    |
| PLL_N                     | 336          | Multiplier                       |
| PLL_P                     | 2            | Output divider for SYSCLK        |
| PLL_Q                     | 7            | For USB, RNG, etc.               |
| VDD                       | 3.3 V        | Core voltage                     |
| Regulator Output          | Scale1 mode  | Max performance power scale      |
| Flash Latency             | 5 WS         | Flash wait states for 168 MHz    |

### Flash Memory Map (STM32F4 - 1MB Flash)

| Sector | Component                      | Start          | Size     | End            | Notes                                         |
| :----: | ------------------------------ | -------------- | -------- | -------------- | --------------------------------------------- |
|    0   | Bootloader I (Primary)         | 0x08000000     | 16 KB    | 0x08003FFF     | Main bootloader                               |
|    1   | Private Certificate            | 0x08004000     | 16 KB    | 0x08007FFF     | Secure/private key storage                    |
|    2   | Error Logs                     | 0x08008000     | 16 KB    | 0x0800BFFF     | Runtime or diagnostic logs                    |
|    3   | Configuration & Flags          | 0x0800C000     | 16 KB    | 0x0800FFFF     | Boot configuration and flags                  |
|    4   | Bootloader II (Secondary)      | 0x08010000     | 64 KB    | 0x0801FFFF     | Secondary bootloader                          |
|    4   | Bootloader II Metadata         | 0x0801FC00     | 512 B    | 0x0801FDFF     | Metadata block inside sector 4                |
|    5   | Main Application               | 0x08020000     | 128 KB   | 0x0803FFFF     | Application (part 1)                          |
|    6   | Main Application               | 0x08040000     | 128 KB   | 0x0805FFFF     | Application (part 2)                          |
|    7   | Main Application               | 0x08060000     | 127 KB   | 0x0807FBFF     | Application (part 3, excluding reserved area) |
|    7   | App Metadata                   | 0x0807FC00     | 512 B    | 0x0807FDFF     | Metadata for current application              |
|    7   | App Certificate                | 0x0807FE00     | 512 B    | 0x0807FFFF     | Firmware certificate for current app          |
|    8   | Reserved (lower half)          | 0x08080000     | 64 KB    | 0x0808FFFF     | Reserved for future use                       |
|    8   | New Bootloader II (upper half) | 0x08090000     | 64 KB    | 0x0809FFFF     | Used during firmware updates                  |
|    8   | New Bootloader II Metadata     | 0x0809FC00     | 512 B    | 0x0809FDFF     | Metadata for new secondary bootloader         |
|    9   | New Application                | 0x080A0000     | 128 KB   | 0x080BFFFF     | New app (part 1)                              |
|   10   | New Application                | 0x080C0000     | 128 KB   | 0x080DFFFF     | New app (part 2)                              |
|   11   | New Application                | 0x080E0000     | 127 KB   | 0x080FFBFF     | New app (part 3, excluding reserved area)     |
|   11   | New App Metadata               | 0x080FFC00     | 512 B    | 0x080FFDFF     | Metadata for new application                  |
|   11   | New App Certificate            | 0x080FFE00     | 512 B    | 0x080FFFFF     | Firmware certificate for new app              |
|    —   | **Flash Total**                | **0x08000000** | **1 MB** | **0x080FFFFF** | Entire internal flash region                  |

[*Stay tuned for updates as the project evolves!*](https://team3m.atlassian.net/jira/software/projects/HA/boards/2/backlog)
