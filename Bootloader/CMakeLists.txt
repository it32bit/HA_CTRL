cmake_minimum_required(VERSION 3.22)

include("${CMAKE_SOURCE_DIR}/cmake/vscode_generated.cmake")

# Bootloader target
set(BOOT_TARGET ha-ctrl-boot)

# Linker script
set(boot_linker_script_SRC ${boot_linker_script_SRC}
    ${CMAKE_SOURCE_DIR}/Bootloader/boot_FLASH.ld
)

# Filter only bootloader-related sources
set(bootloader_sources "")

foreach(src ${sources_SRCS})
    if(src MATCHES ".*/Bootloader/.*")
        list(APPEND bootloader_sources ${src})
    elseif(src MATCHES ".*/Platform/STM32F4/Src/.*")
        list(APPEND bootloader_sources ${src})
    elseif(src MATCHES ".*/Core/Src/system_stm32f4xx.c")
        list(APPEND bootloader_sources ${src})
    elseif(src MATCHES ".*/Core/Src/syscall.c")
        list(APPEND bootloader_sources ${src})
    elseif(src MATCHES ".*/Startup/startup_stm32f407vgtx.s")
        list(APPEND bootloader_sources ${src})
    elseif(src MATCHES ".*/Drivers/stm32f4xx-hal-driver/Src/.*")
        list(APPEND bootloader_sources ${src})
    endif()
endforeach()

message(STATUS "Building target ${BOOT_TARGET} with sources:")
foreach(file IN LISTS bootloader_sources)
    message(STATUS "> ${file}")
endforeach()

# Create executable first
add_executable(${BOOT_TARGET}
    ${bootloader_sources}
)

# Link with PlatformSTM32F4 HAL wrapper library
target_link_libraries(${BOOT_TARGET} PRIVATE
    PlatformSTM32F4
)

# Set output directory for ELF, BIN, and HEX files
set_target_properties(${BOOT_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR_BIN}
)

# Include paths
target_include_directories(${BOOT_TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers//stm32f4xx-hal-driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/cmsis-device-f4/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Bootloader/Inc
    ${CMAKE_SOURCE_DIR}/Platform/Interface
    ${CMAKE_SOURCE_DIR}/Platform/STM32F4/Inc
)

# Compiler defines
target_compile_definitions(${BOOT_TARGET} PRIVATE
    STM32F407xx
    USE_HAL_DRIVER
    USE_FULL_LL_DRIVER
)

# Linker script (set externally from parent CMakeLists)
target_link_options(${BOOT_TARGET} PRIVATE
    -T${boot_linker_script_SRC}
    ${cpu_PARAMS}
    ${linker_OPTS}
    -Wl,--gc-sections -Wl,-Map=${CMAKE_BINARY_DIR_BIN}/${BOOT_TARGET}.map
    --specs=nosys.specs
    -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
    -Wl,-z,max-page-size=8
    -Wl,--print-memory-usage
)

# Compiler options
target_compile_options(${BOOT_TARGET} PRIVATE
    ${cpu_PARAMS}
    ${compiler_OPTS}
    -Wall -Wextra -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -MMD -MP>
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-Og -g0>
)

# Create bin and hex after build
add_custom_command(TARGET ${BOOT_TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${BOOT_TARGET}> ${CMAKE_BINARY_DIR_BIN}/${BOOT_TARGET}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${BOOT_TARGET}> ${CMAKE_BINARY_DIR_BIN}/${BOOT_TARGET}.hex
    COMMENT "Generating HEX and BIN for Bootloader"
)

# Print memory usage
add_custom_command(TARGET ${BOOT_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "******** Print size information:"
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${BOOT_TARGET}>
    COMMENT "Size information"
)
