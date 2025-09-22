# This is converter generated file, and shall not be touched by user
#
# Use CMakeLists.txt to apply user changes
cmake_minimum_required(VERSION 3.22)

# Core MCU flags, CPU, instruction set and FPU setup
set(cpu_PARAMS ${cpu_PARAMS}
    -mthumb

    # Other parameters
    # -mcpu, -mfloat, -mfloat-abi, ...
    -mcpu=cortex-m4
	-mfpu=fpv4-sp-d16
	-mfloat-abi=hard

)

# Linker script
set(linker_script_SRC ${linker_script_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f407vgtx_FLASH.ld
)

# Sources
set(sources_SRCS ${sources_SRCS}

	${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/main.c
	${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/syscall.c
	${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/sysmem.c
	${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32f4xx.c
	${CMAKE_CURRENT_SOURCE_DIR}/Startup/startup_stm32f407vgtx.s
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal.c
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_rcc.c
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_gpio.c
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_cortex.c
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Src/stm32f4xx_hal_rcc_ex.c
)

# Include directories
set(include_c_DIRS ${include_c_DIRS}

	${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/stm32f4xx-hal-driver/Inc
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/cmsis-device-f4/Include
	${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Core/Include
)
set(include_cxx_DIRS ${include_cxx_DIRS}

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

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
