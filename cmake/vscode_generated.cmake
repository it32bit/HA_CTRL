# This is converter generated file, and shall not be touched by user
#
# Use CMakeLists.txt to apply user changes
cmake_minimum_required(VERSION 3.22)

# Core MCU flags, CPU, instruction set and FPU setup
set(cpu_PARAMS ${cpu_PARAMS}
    -mthumb
    -mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard
)

# Sources
set(sources_SRCS ${sources_SRCS}
	${CMAKE_SOURCE_DIR}/App/Src/app.cpp
	${CMAKE_SOURCE_DIR}/App/Src/app_it.cpp
	${CMAKE_SOURCE_DIR}/App/Src/cfg_gpio.cpp
	${CMAKE_SOURCE_DIR}/App/Src/disp_gpio.cpp
	${CMAKE_SOURCE_DIR}/App/Src/hal_uart.cpp
	${CMAKE_SOURCE_DIR}/App/Src/hal_adc.cpp
	${CMAKE_SOURCE_DIR}/App/Src/console.cpp
	${CMAKE_SOURCE_DIR}/App/Src/watchdog.cpp
	${CMAKE_SOURCE_DIR}/Bootloader/Src/boot.cpp
	${CMAKE_SOURCE_DIR}/Platform/STM32F4/Src/hal_gpio.cpp
	${CMAKE_SOURCE_DIR}/Core/Src/syscall.c
	${CMAKE_SOURCE_DIR}/Core/Src/sysmem.c
	${CMAKE_SOURCE_DIR}/Core/Src/system_stm32f4xx.c
	${CMAKE_SOURCE_DIR}/Core/Src/stm32f4xx_it.c
	${CMAKE_SOURCE_DIR}/Startup/startup_stm32f407vgtx.s
	${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal.c
	${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_rcc.c
	${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_gpio.c
	${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_cortex.c
	${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_rcc_ex.c
	${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_ll_exti.c
	${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_ll_rcc.c
	${CMAKE_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_ll_utils.c
)

# Include directories
set(include_HEADERS_DIRS ${include_HEADERS_DIRS}

)

set(include_c_DIRS ${include_c_DIRS}
    ${include_HEADERS_DIRS}
)

set(include_cxx_DIRS ${include_cxx_DIRS}
	${include_HEADERS_DIRS}
)

set(include_asm_DIRS ${include_asm_DIRS}

)

# Symbols definition
set(symbols_c_SYMB ${symbols_c_SYMB}

)
set(symbols_cxx_SYMB ${symbols_cxx_SYMB}

)
set(symbols_asm_SYMB ${symbols_asm_SYMB}

)

# Link directories
set(link_DIRS ${link_DIRS}

)

# Link libraries
set(link_LIBS ${link_LIBS}

)

# Compiler options
set(compiler_OPTS ${compiler_OPTS})

# Linker options
set(linker_OPTS ${linker_OPTS})

# Generating compile_command.jsn
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compilation commands into compile_commands.json")
