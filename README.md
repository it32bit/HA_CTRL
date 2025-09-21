# CPP_HA_CTRL

System Supporting Smart Home Control

## Host

Ubuntu 24.04 Lts
VSCode

## CPU

STM32F407G-DISC1

## Board

STM32F407G-DISC1

## Submodules

git submodule add https://github.com/STMicroelectronics/stm32f4xx-hal-driver.git Drivers/stm32f4xx-hal-driver
git submodule add https://github.com/STMicroelectronics/cmsis-device-f4.git Drivers/cmsis-device-f4

## Linux

    sudo ln -s /opt/stm32cubeclt_1.19.0/STM32CubeProgrammer/bin/STM32_Programmer_CLI /usr/bin/

## VSCode

    VS Code can hide folders based on your settings.
    Fix:
        Open Command Palette (Ctrl+Shift+P) → type Preferences: Open Settings (JSON) and check for missing folders.

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

## GNU Tools for STM32

$ /opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin/arm-none-eabi-gcc --version

arm-none-eabi-gcc (GNU Tools for STM32 13.3.rel1.20240926-1715) 13.3.1 20240614
Copyright (C) 2023 Free Software Foundation, Inc.
