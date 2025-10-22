cmake_minimum_required(VERSION 3.22)

include("${CMAKE_SOURCE_DIR}/cmake/platform-traits.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/project-sources.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/compiler-warnings.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/utilities.cmake")

# Primary Bootloader target
set(BPRIM_TARGET ha-ctrl-prim)

# Linker script
set(bprim_linker_script_SRC ${bprim_linker_script_SRC}
    ${CMAKE_SOURCE_DIR}/BootPrim/boot_prim_linker.ld
)

# Filter only bootloader-related sources
set(boot_prim_sources "")

foreach(src ${sources_SRCS})
    if(src MATCHES ".*/Boot/.*" OR
       src MATCHES ".*/BootPrim/.*" OR
       src MATCHES ".*/Platform/STM32F4/Src/.*" OR
       src MATCHES ".*/Core/Src/system_stm32f4xx.c" OR
       src MATCHES ".*/Core/Src/syscall.c" OR
       src MATCHES ".*/Startup/startup_stm32f407vgtx.s" OR
       src MATCHES ".*/Drivers/stm32f4xx-hal-driver/Src/.*")
        list(APPEND boot_prim_sources ${src})
    endif()
endforeach()

message(STATUS "Building target ${BPRIM_TARGET} with sources:")
foreach(file IN LISTS boot_prim_sources)
    message(STATUS "[prim] ${file}")
endforeach()

# Create executable first
add_executable(${BPRIM_TARGET}
    ${boot_prim_sources}
)

# Link with Platform_STM32F4 HAL wrapper library
target_link_libraries(${BPRIM_TARGET} PRIVATE
    Platform_STM32F4
)

# Set output directory for ELF, BIN, and HEX files
set_target_properties(${BPRIM_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR_BIN}
)

# Include paths
target_include_directories(${BPRIM_TARGET} PRIVATE
    ${MCU_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Platform/Interface
    ${CMAKE_SOURCE_DIR}/Boot/Inc
    ${CMAKE_SOURCE_DIR}/BootSec/Inc
)

# Compiler defines
target_compile_definitions(${BPRIM_TARGET} PRIVATE
    BOOT_PRIM
    USE_HAL_DRIVER
    ${MCU_DEFINES}
)

# Linker script (set externally from parent CMakeLists)
target_link_options(${BPRIM_TARGET} PRIVATE
    -T${bprim_linker_script_SRC}
    ${cpu_PARAMS}
    ${linker_OPTS}
    -Wl,-Map=${CMAKE_BINARY_DIR_BIN}/${BPRIM_TARGET}.map
    -Wl,--gc-sections
    --specs=nosys.specs
    -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
    -Wl,-z,max-page-size=8
    -Wl,--print-memory-usage
    -u _printf_float
)

# CPU/Compiler specific flags and optimizations
target_compile_options(${BPRIM_TARGET} PRIVATE
    ${cpu_PARAMS}
    ${compiler_OPTS}
)

# Set common warning flags, see cmake/compiler-warnings.cmake
set_project_warnings(${BPRIM_TARGET})

# Create bin and hex after build
add_custom_command(TARGET ${BPRIM_TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${BPRIM_TARGET}> ${CMAKE_BINARY_DIR_BIN}/${BPRIM_TARGET}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${BPRIM_TARGET}> ${CMAKE_BINARY_DIR_BIN}/${BPRIM_TARGET}.hex
    COMMENT "Generating HEX and BIN for Bootloader"
)

# Print memory usage
add_size_print(${BPRIM_TARGET})

# Flash target (using ST-Link via STM32CubeProgrammer)
add_custom_target(boot-prim-flash
    DEPENDS ${BPRIM_TARGET}
    COMMAND ${STM32CUBE_PROGRAMMER} -c port=SWD -d ${CMAKE_BINARY_DIR_BIN}/${BPRIM_TARGET}.elf
    COMMENT "Flashing Bootloader via ST-Link"
)
