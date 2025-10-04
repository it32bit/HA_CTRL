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

## INFO-18 Size after serialDebug

```bash
[20/20] Linking CXX executable ~/repos/ha-ctrl/bin/ha-ctrl.elf
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4216 B       128 KB      3.22%
           FLASH:       30380 B         1 MB      2.90%
******** Print size information:
   text    data     bss     dec     hex filename
  28540    1832    2384   32756    7ff4 ~/repos/ha-ctrl/bin/ha-ctrl.elf
```

## INFO-18 Size after Observer pattern

```bash
[20/20] Linking CXX executable ~/repos/ha-ctrl/bin/ha-ctrl.elf
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4320 B       128 KB      3.30%
           FLASH:       80000 B         1 MB      7.63%
******** Print size information:
   text    data     bss     dec     hex filename
  78144    1848    2472   82464   14220 ~/repos/ha-ctrl/bin/ha-ctrl.elf
```

## Common C++ Naming Conventions

Types start with upper case: `MyClass`.
Functions and variables start with lower case: `myMethod`.
Constants are all upper case: `const double PI=3.14159265358979323;`.

C++ Standard Library use these guidelines:

Macro names use upper case with underscores: `INT_MAX`.
Template parameter names use Pascal case: `InputIterator`.
All other names use snake case: `unordered_map`.

## Distinguish Private Object Data

Name private data with a `m_` prefix to distinguish it from public data. `m_` stands for "member" data.

## Distinguish Function Parameters

The most important thing is consistency within your codebase; this is one possibility to help with consistency.

Name function parameters with an `t_` prefix. `t_` can be thought of as "the", but the meaning is arbitrary. The point is to distinguish function parameters from other variables in scope while giving us a consistent naming strategy.

```cpp
struct Size
{
  int width;
  int height;

  Size(int t_width, int t_height) : width(t_width), height(t_height) {}
};

// This version might make sense for thread safety or something,
// but more to the point, sometimes we need to hide data, sometimes we don't.
class PrivateSize
{
  public:
    int width() const { return m_width; }
    int height() const { return m_height; }
    PrivateSize(int t_width, int t_height) : m_width(t_width), m_height(t_height) {}

  private:
    int m_width;
    int m_height;
};
```

## Never Use `using namespace` in a Header File

This causes the namespace you are `using` to be pulled into the namespace of all files that include the header file.
It pollutes the namespace and it may lead to name collisions in the future.
Writing `using namespace` in an implementation file is fine though.

## Include Guards

Header files must contain a distinctly-named include guard to avoid problems with including the same header multiple times and to prevent conflicts with headers from other projects.

You may also consider using the `#pragma once` directive instead which is quasi-standard across many compilers.
It's short and makes the intent clear.

### GCC Fuses

`-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic` - use these and consider the following (see descriptions below)

`-pedantic` - Warn on language extensions
`-Wall -Wextra` reasonable and standard
`-Wshadow` warn the user if a variable declaration shadows one from a parent context
`-Wnon-virtual-dtor` warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
`-Wold-style-cast` warn for c-style casts
`-Wcast-align` warn for potential performance problem casts
`-Wunused` warn on anything being unused
`-Woverloaded-virtual` warn if you overload (not override) a virtual function
`-Wpedantic` (all versions of GCC, Clang >= 3.2) warn if non-standard C++ is used
`-Wconversion` warn on type conversions that may lose data
`-Wsign-conversion` (Clang all versions, GCC >= 4.3) warn on sign conversions
`-Wmisleading-indentation` (only in GCC >= 6.0) warn if indentation implies blocks where blocks do not exist
`-Wduplicated-cond` (only in GCC >= 6.0) warn if `if` / `else` chain has duplicated conditions
`-Wduplicated-branches` (only in GCC >= 7.0) warn if `if` / `else` branches have duplicated code
`-Wlogical-op` (only in GCC) warn about logical operations being used where bitwise were probably wanted
`-Wnull-dereference` (only in GCC >= 6.0) warn if a null dereference is detected
`-Wuseless-cast` (only in GCC >= 4.8) warn if you perform a cast to the same type
`-Wdouble-promotion` (GCC >= 4.6, Clang >= 3.8) warn if `float` is implicitly promoted to `double`
`-Wformat=2` warn on security issues around functions that format output (i.e., `printf`)
`-Wlifetime` (only special branch of Clang currently) shows object lifetime issues
`-Wimplicit-fallthrough` Warns when case statements fall-through. (Included with `-Wextra` in GCC, not in clang)

Consider using `-Weverything` and disabling the few warnings you need to on Clang

`-Weffc++` warning mode can be too noisy, but if it works for your project, use it also.

## INFO-18 Size after added Console Basic concept

```bash
[21/21] Linking CXX executable ~/repos/ha-ctrl/bin/ha-ctrl.elf
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4320 B       128 KB      3.30%
           FLASH:       83324 B         1 MB      7.95%
******** Print size information:
   text    data     bss     dec     hex filename
  81468    1848    2472   85788   14f1c ~/repos/ha-ctrl/bin/ha-ctrl.elf
  ```

## Summary of `constexpr`

| Approach                     | Flash-safe | Multiple TU safe | constexpr       |
|-----------------------------|------------|------------------|-----------------|
| inline `constexpr` in header | ok     | ok           | ok          |
| `extern const` + cpp define  | ok     | ok           | x          |
| `extern constexpr`           | x     | x           | x *(invalid)*  |

## INFO-19 GIT Local Hook

Configure Git to prevent committing staged code without a commit message by using a commit-msg hook.

```bash
cd .git/hooks

touch commit-msg
chmod +x commit-msg

#!/bin/sh
if test -z "$1" || ! grep -q '[^[:space:]]' "$1"; then
  echo "Commit message cannot be empty."
  exit 1
fi
```

What This Does

- Reads the commit message from the temporary file Git creates.
- Checks if it's empty or only whitespace.
- If so, it blocks the commit and shows an error.

## INFO-20 Git Global Hooks

Use a Git Template (Native Git)

This lets you share hooks across projects by setting up a global template.

Step-by-step:

```bash
mkdir -p ~/.git-templates/hooks

nano ~/.git-templates/hooks/commit-msg

#!/bin/sh
msg=$(cat "$1")
if [ -z "$msg" ] || ! echo "$msg" | grep -q '[^[:space:]]'; then
  echo "ommit message cannot be empty."
  exit 1
fi

chmod +x ~/.git-templates/hooks/commit-msg

git config --global init.templateDir ~/.git-templates

cd your-project
git init
```

Now every new repo you git init will include this hook automatically.

## INFO-21 Refactoring Observer Pattern for Console

Goal:

- Observers are registered at compile time
- Notifications are fast and safe inside interrupts
- Everything is constexpr-friendly and flash-resident

Howto:

- Define a Static Observer Interface
- Create a Fixed Observer List

Why This Is Embedded-Safe

- No heap, no dynamic dispatch
- No runtime registration
- All data lives in .rodata (flash)
- ISR-safe: fast, predictable, no blocking

Old Size: `consoleObserver.notify(static_cast<uint8_t>(t_byte));`

```bash
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4296 B       128 KB      3.28%
           FLASH:       81440 B         1 MB      7.77%
******** Print size information:
   text    data     bss     dec     hex filename
  79584    1848    2448   83880   147a8 ~/repos/ha-ctrl/bin/ha-ctrl.elf
```

New Size: `uart2_Observers.notifyAll(static_cast<uint8_t>(t_byte));`

```bash
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4408 B       128 KB      3.36%
           FLASH:       80864 B         1 MB      7.71%
******** Print size information:
   text    data     bss     dec     hex filename
  79008    1848    2560   83416   145d8 ~/repos/ha-ctrl/bin/ha-ctrl.elf
```

Code size for `uart2_Observers.notifyAll(...)` is the better approach in terms of:

- Compile-time safety
- Flash residency
- ISR compatibility

Last Size `uart2_Observers(staticObservers, sizeof(staticObservers) / sizeof(staticObservers[0]));`

```bash
Memory region         Used Size  Region Size  %age Used
          CCMRAM:           0 B        64 KB      0.00%
             RAM:        4408 B       128 KB      3.36%
           FLASH:       80880 B         1 MB      7.71%
******** Print size information:
   text    data     bss     dec     hex filename
  79024    1848    2560   83432   145e8 ~/repos/ha-ctrl/bin/ha-ctrl.elf
```

### Summary Size comparation for refactoring Observer Pattern for Console

| Version / Call Style                                 | .text  | .data | .bss  | RAM Used | FLASH Used | Total Size | % RAM Used | % FLASH Used | Change vs. Old |
|------------------------------------------------------|--------|-------|-------|----------|------------|------------|------------|---------------|----------------|
| `consoleObserver.notify(...)`                        | 79584  | 1848  | 2448  | 4296 B   | 81440 B    | 83880 B    | 3.28%      | 7.77%         | —              |
| `uart2_Observers.notifyAll(...)` (with std::array)   | 79008  | 1848  | 2560  | 4408 B   | 80864 B    | 83416 B    | 3.36%      | 7.71%         | −464 B total   |
| `uart2_Observers(...)` (with raw array)              | 79024  | 1848  | 2560  | 4408 B   | 80880 B    | 83432 B    | 3.36%      | 7.71%         | −448 B total   |

**Legend:**

- `.text`: Executable code section stored in FLASH.
- `.data`: Initialized global/static variables stored in RAM.
- `.bss`: Uninitialized global/static variables stored in RAM.
- **RAM Used** = `.data` + `.bss`
- **FLASH Used** = `.text` + `.rodata` (not shown separately)
- **Total Size** = `.text` + `.data` + `.bss`
- **% RAM/FLASH Used**: Based on total available (128 KB RAM, 1 MB FLASH)
- **Change vs. Old**: Difference compared to original `consoleObserver` version.

`uart2_Observers(...)` (with raw array)`

| Reason                       | Explanation                                                                                      |
|------------------------------|--------------------------------------------------------------------------------------------------|
| Zero RAM usage               | Uses raw `constexpr` array stored entirely in `.rodata`, avoiding `.bss` or `.data` allocations. |
| FLASH-resident               | Observer list and metadata live in flash, reducing runtime memory footprint.                    |
| Compile-time safety          | Observer list size and contents are resolved at compile time, preventing runtime errors.         |
| ISR compatibility            | No dynamic memory, no virtual dispatch — safe for use inside interrupt service routines.         |
| Minimal code size            | Smaller `.text` footprint compared to `std::array` or singleton-based dispatch.                  |
| Transparent memory layout    | Easy to verify in `.map` file; no hidden allocations or runtime surprises.                      |
| Simple and predictable       | Straightforward structure with no STL overhead, ideal for resource-constrained environments.     |

## Message to Merge-Request

Constexpr-safe command table for storing in flash memory, is to avoid runtime-managed types like std::function, std::vector, and std::string_view.

This Is Flash-Safe
All data is compile-time constant (constexpr) and stored in .rodata section
No heap allocation (dynamic memory), no runtime-managed types
Can be placed in .rodata or .text section of flash
Ideal for embedded systems with strict memory constraints

Console:
A fixed-size character buffer
A receivedData() method that handles input byte-by-byte
Message termination detection (\r or \n)
Integration with the process() function

In C++, a constexpr variable must be fully defined at the point of declaration. You cannot declare a constexpr array with extern unless it's also immediately initialized — which defeats the purpose of extern.
The Rule: constexpr variables are implicitly inline, and must be defined, not just declared.

## INFO-22 Clang-Tidy is a command line tool that performs static analysis on C and C++ source code

Install Clang-Tidy

```bash
sudo wget -qO /usr/local/bin/clang-tidy https://github.com/cpp-linter/clang-tools-static-binaries/releases/latest/download/clang-tidy-20_linux-amd64

sudo chmod a+x /usr/local/bin/clang-tidy

clang-tidy --version

LLVM (http://llvm.org/):
  LLVM version 20.1.0
```

Uninstall Clang-Tidy

```bash
sudo rm -rf /usr/local/bin/clang-tidy
```

find App/Src -name '*.cpp' | xargs -I{} clang-tidy {} -p build

## INFO-22 Clang-Tidy Execution

It was very hard to settings all together to be working.
Check comments inside CMakeList.

- Before start using first compile `CMakeList.txt` file:

  ```bash
  cmake -S . -B build/debug -G Ninja
  ```

- Start execution:

  ```bash
  cmake --build build/debug --target clang_tidy_log
  ```

In `CMakeList.txt` there is a target added `clang_tidy_log`
it will execute clang-tidy on /App/* directory and create Log file
`clang-tidy-warn.log` with all warnings/errors set in .clang-tidy.

```bash
~/repos/ha-ctrl/bin/clang-tidy-warn.log
```

## INFO-23 Extension - cs128.cs128-clang-tidy

File → Preferences → Settings → Extensions → Clang-Tidy

- clang-tidy.executable: /usr/bin/clang-tidy
- clang-tidy.buildPath: /home/kojot/repos/ha-ctrl/build/debug
- clang-tidy.lintOnSave: Enable

Run Clang-Tidy in VSCode:

- Open a .cpp file in /App
- Save the file
- Diagnostics will appear in the Problems tab and inline in the editor

Optionaly: Exclude Files

```json
"clang-tidy.blacklist": [
  ".*test.*",
  ".*vendor.*"
]
```

## INFO-24

Create a virtual environment

```bash
python3 -m venv ~/venvs/clangtidy
```

Activate virtual environment

```bash
source ~/venvs/clangtidy/bin/activate
```

Install the tool on virtual environment

```bash
(clangtidy) it32bit@it32bit:~$ python3 -m pip install clang-html
```

In reported log there are lots of artefacts:

- Progress headers (--- Running Clang-Tidy on ... ---)
- Summary lines (2 warnings generated.)
- Notes and errors without consistent formatting

Clenup

```bash
(clangtidy) it32bit@it32bit:~$
find ./App -name '*.cpp' | xargs -I{} clang-tidy {} -p ./build/debug --quiet --extra-arg=-fno-color-diagnostics > ./clang-tidy-clean.log 2>&1
```

Generate the HTML Report

```bash
(clangtidy) it32bit@it32bit:~$ python3 -m clang_html ./clang-tidy-clean.log -o ./clang-tidy-report.html
```

## INFO-25 Clangd

For better check clang-tidy: clangd was installed

```bash
clangd --version
Ubuntu clangd version 18.1.3 (1ubuntu1)
Features: linux+grpc
Platform: x86_64-pc-linux-gnu
```

Extension for VS Code:

- C/C++ - Disable
- ClangD - Enabled
