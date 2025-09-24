# Log Book

This logbook documents the issues and challenges I encountered during the development of the project.
It contains raw facts and console listings, often without clear references to the source files.
Most entries are taken directly from terminal output.
Additionally, it includes quick fix rules for resolving problems, written without editing or attention to grammar.

## ISSUE-01

The C compiler "/usr/bin/arm-none-eabi-gcc" is not able to compile a simple test program. The error log is as follows:

```bash
    CMake Error at /usr/share/cmake-3.28/Modules/CMakeTestCCompiler.cmake:67 (message):The C compiler

    "/usr/bin/arm-none-eabi-gcc"

    is not able to compile a simple test program.

    It fails with the following output:

    Change Dir: '/home/kojot/repos/ha-ctrl/build/debug/CMakeFiles/CMakeScratch/TryCompile-LsxTNo'

    Run Build Command(s): /usr/bin/ninja -v cmTC_f2ca9
    [1/2] /usr/bin/arm-none-eabi-gcc   -fdata-sections -ffunction-sections -Wl,--gc-sections -o CMakeFiles/cmTC_f2ca9.dir/testCCompiler.c.obj -c /home/kojot/repos/ha-ctrl/build/debug/CMakeFiles/CMakeScratch/TryCompile-LsxTNo/testCCompiler.c
    FAILED: CMakeFiles/cmTC_f2ca9.dir/testCCompiler.c.obj
    /usr/bin/arm-none-eabi-gcc   -fdata-sections -ffunction-sections -Wl,--gc-sections -o CMakeFiles/cmTC_f2ca9.dir/testCCompiler.c.obj -c /home/kojot/repos/ha-ctrl/build/debug/CMakeFiles/CMakeScratch/TryCompile-LsxTNo/testCCompiler.c
    arm-none-eabi-gcc: fatal error: cannot execute 'cc1': execvp: No such file or directory
    compilation terminated.
    ninja: build stopped: subcommand failed.

    CMake will not be able to correctly generate this project.
```

SOLUTION:
    The READONLY keyword has been removed from all relevant section definitions in linker script
    for compatibility with GCC10 and earlier.
    stm32f407vgtx_FLASH.ld file is updated.

## ISSUE-02

CMake Error at cmake/gcc-arm-none-eabi.cmake:19 (set):

```bash
    Build cmake error log:
    "Tell CMake where to find the compiler by setting either the environment variable CC
    or the CMake cache entry CMAKE_C_COMPILER to the full path to the compiler,
    or to the compiler name if it is in the PATH."
```

SOLUTION:
The path to the ARM GCC toolchain is now explicitly set in cmake/gcc-arm-none-eabi.cmake file.
set(TOOLCHAIN_PATH "/opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin")
and all relevant CMake variables are updated to use this path.

arm-none-eabi-gcc --version
arm-none-eabi-gcc (GNU Tools for STM32 13.3.rel1.20240926-1715) 13.3.1 20240614

## ISSUE-03

VS Code can hide folders based on your settings /.vscode.
Fix: Open Command Palette (Ctrl+Shift+P) → type Preferences: Open Settings (JSON) and check for missing folders.

## ISSUE-04

Common issue in VS Code when IntelliSense doesn't pick up preprocessor definitions that are correctly passed to the compiler via CMake.
Sections like #ifdef MY_DEFINICTION are properly compiled but not recognized and not shadowed in vscode:
Fix: IntelliSense Preprocessor Defines in VS Code:
Enable CMake Configuration Provider In your .vscode/settings.json, add

```json
{
"CMake.configureOnOpen": true,
"CMake: UseCMakePresets": "always"
}
```

Make sure your CMake build generates a compile_commands.json file: `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`
Point VS Code to compile_commands.json in .vscode/c_cpp_properties.json, set:

```json
    "configurations": [
        {
        ...
        "defines": [
            "HAL_LIB"
        ],
        "compileCommands": "${workspaceFolder}/build/debug/compile_commands.json"
        }
    ]
```

## ISSUE-05

Solution for incorrect toolchain path:
[Fille:cmake/gcc-arm-none-eabi.cmake]
set(TOOLCHAIN_PATH "/opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin")

## ISSUE-06

Solution for TAG: "READONLY" in *_FLASH.ld  file. The "READONLY" keyword is only supported in GCC11 and later,
remove it (in whole file) if using GCC10 or earlier.
[Fille:stm32f407vgtx_FLASH.ld]
...
.init_array (READONLY) :
...

## ISSUE-07

Solution for missing path headers from stm3244xx_hal.h in *.{cpp, hpp} files
Section "include_c_DIRS" & "include_cxx_DIRS" --should contain the same folders--

```C
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
```

## ISSUE-08

Solution for error: unknown type name 'class' in app_main.hpp when included in main.c
Fix: Move all class LedController to a separate api_gpio.hpp file and expose only C-compatible functions in the header.

## WARN-09 Related to C++

```C
using GpioAction = void(*)(GPIO_TypeDef*, uint32_t);

constexpr void noop(GPIO_TypeDef*, uint32_t) {}
constexpr void setLow(GPIO_TypeDef* gpio, uint32_t pin) {
    LL_GPIO_ResetOutputPin(gpio, pin);
}
constexpr void setHigh(GPIO_TypeDef* gpio, uint32_t pin) {
    LL_GPIO_SetOutputPin(gpio, pin);
}
constexpr std::array<GpioAction, 3> dispatch = { noop, setLow, setHigh };

```

~/repos/ha-ctrl/App/Inc/hal_gpio.hpp: In function 'constexpr void setOutputLow(GPIO_TypeDef*, uint32_t)':
~/repos/ha-ctrl/App/Inc/hal_gpio.hpp:89:27: warning: call to non-'constexpr' function 'void LL_GPIO_ResetOutputPin(GPIO_TypeDef*, uint32_t)' [-Winvalid-constexpr]
   89 |     LL_GPIO_ResetOutputPin(gpio, pin);

```bash
**constexpr** means “can be evaluated at compile time.”
But hardware access functions like LL_GPIO_ResetOutputPin() are **runtime-only** — they read/write memory-mapped registers, which the compiler cannot simulate during compilation.
```

Summary:
    - constexpr → not allowed for hardware access.
    - inline → safe and efficient for embedded headers.
    - static → optional if you want internal linkage in a .cpp file.
