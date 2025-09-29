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

## WARN-10 Related to C++

```bash
[14/14] Linking CXX executable ~/repos/ha-ctrl/bin/ha-ctrl.elf
/opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin/../lib/gcc/arm-none-eabi/13.3.1/../../../../arm-none-eabi/bin/ld: /opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin/../lib/gcc/arm-none-eabi/13.3.1/../../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(libc_a-getentropyr.o): in function `_getentropy_r':
(.text._getentropy_r+0xe): warning: _getentropy is not implemented and will always fail
/opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin/../lib/gcc/arm-none-eabi/13.3.1/../../../../arm-none-eabi/bin/ld: /opt/stm32cubeclt_1.19.0/GNU-tools-for-STM32/bin/../lib/gcc/arm-none-eabi/13.3.1/../../../../arm-none-eabi/lib/thumb/v7e-m+fp/hard/libc.a(libc_a-getentropyr.o): note: the message above does not take linker garbage collection into account
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4248 B       128 KB      3.24%
           FLASH:       75432 B         1 MB      7.19%
******** Print size information:
   text    data     bss     dec     hex filename
  73636    1784    2464   77884   1303c ~/repos/ha-ctrl/bin/ha-ctrl.elf
build finished with warning(s).
```

What is _getentropy?
_getentropy is a system-level function typically used to retrieve random data from the operating system. It's part of the POSIX API, but:

STM32 microcontrollers don’t have an operating system by default.
newlib includes a placeholder for _getentropy, but it’s not functional unless you provide your own implementation or use an RTOS with entropy support.

Solution is to:
1.Avoid linking code that uses it. Make sure you’re not using features (may internally call _getentropy) like:

```C
    std::random_device from <random> // in C++
    arc4random() // or other high-level entropy functions
```

2.Provide a stub to silence the warning:

```C
extern "C" int _getentropy(void* buffer, size_t length) {
    return -1; // always fail, It will override the weak symbol from libc.a
}
```

First warning occur in:
    Commit: 3cdf35e7fbd82525eff7aa2b03ea5bee3f99f225
    Date: Thu Sep 25 2025 01:32:08 GMT+0200 (Central European Summer Time)
    Generic GpioManager implemented.

Related to adding `map` array.

## INFO-11 Code Size

```bash
[14/14] Linking CXX executable ~/repos/ha-ctrl/bin/ha-ctrl.elf
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4248 B       128 KB      3.24%
           FLASH:       75432 B         1 MB      7.19%
******** Print size information:
   text    data     bss     dec     hex filename
  73636    1784    2464   77884   1303c ~/repos/ha-ctrl/bin/ha-ctrl.elf
build finished successfully.
```

## INFO-12 Code Size

```bash
[14/14] Linking CXX executable ~/repos/ha-ctrl/bin/ha-ctrl.elf
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        2016 B       128 KB      1.54%
           FLASH:        5428 B         1 MB      0.52%
******** Print size information:
   text    data     bss     dec     hex filename
   5400      28    1988    7416    1cf8 ~/repos/ha-ctrl/bin/ha-ctrl.elf
build finished successfully.
```

Replaced 'map' array to 'pair' array and constexpr for pinLabelDefArray.
Those changes were fix WARN-10 and no need to use stub for _getentropy.

Make pinLabelDefArray constexpr for compile-time evaluation, adjust GpioManager to accept const reference.
Fix WARN-10, no need to use stub for _getentropy.

## INFO-13 `std::string_view`

Why use `std::string_view` instead of `std::string` in in `std::pair<std::string_view, size_t>`

Use of std::string_view is a deliberate choice for performance and efficiency, especially in contexts like compile-time mapping or read-only lookup tables.

1. No ownership or allocation
-std::string_view is a lightweight, non-owning view into a string.
-It doesn’t allocate memory or copy data — it just points to an existing string buffer.

2. Faster and cheaper
-Ideal for read-only access to string data.
-Much faster than std::string when you don’t need to modify or store the string.

3. Works well with constexpr
-std::string is not constexpr-friendly due to dynamic allocation.
-std::string_view can be used in constexpr contexts, making it perfect for compile-time.

4. Avoids unnecessary copies
If you use std::string, each entry would copy the string into the container.
With string_view, it just references the original string literal.

Use std::string_view when:
 -You’re working with string literals or static strings.
 -You want fast, read-only access.
 -You’re building constexpr data structures.

## INFO-14 `ls -l /dev/ft232`

Create a new file:

```bash
 sudo nano /etc/udev/rules.d/99-ft232.rules
```

```C
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", SYMLINK+="ft232"
```

```C
SUBSYSTEM=="tty", ATTRS{serial}=="FTxxxxxxxx", SYMLINK+="ft232"
```

```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
ls -l /dev/ft232

lrwxrwxrwx 1 root root 7 Sep 28 17:48 /dev/ft232 -> ttyUSB0
```

## INFO-16 Size comparation of using: `uart_send_string()`, `printf()` and `std::cout << "Text"`

Size without redirection, uart2_send_string("string")

```bash
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        2104 B       128 KB      1.61%
           FLASH:        6744 B         1 MB      0.64%
******** Print size information:
   text    data     bss     dec     hex filename
   6652      92    2012    8756    2234 ~/repos/ha-ctrl/bin/ha-ctrl.elf
```

Size when redirection _write for printf("string"):

```bash
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4216 B       128 KB      3.22%
           FLASH:       30428 B         1 MB      2.90%
******** Print size information:
   text    data     bss     dec     hex filename
  28588    1832    2384   32804    8024 ~/repos/ha-ctrl/bin/ha-ctrl.elf
```

Size when redirection _write for std::cout << "string" :

```bash
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:       10248 B       128 KB      7.82%
           FLASH:      338472 B         1 MB     32.28%
******** Print size information:
   text    data     bss     dec     hex filename
 336376    2088    8160  346624   54a00 ~/repos/ha-ctrl/bin/ha-ctrl.elf
```

## INFO-17 VS Code stickyScroll

It keeps function headers, #ifdef blocks, or bracketed sections "stuck"
to the top of the editor while scrolling so you always know what scope you're in.

```json
"editor.stickyScroll.enabled": true,
```
