cmake_minimum_required(VERSION 3.22)

include("${CMAKE_SOURCE_DIR}/cmake/vscode_generated.cmake")

# App target
set(APP_TARGET ha-ctrl-app)

# Linker script
set(app_linker_script_SRC ${app_linker_script_SRC}
    ${CMAKE_SOURCE_DIR}/App/app_FLASH.ld
)


# Filter only app-related sources
set(app_sources "")

foreach(src ${sources_SRCS})
    if(src MATCHES ".*/App/.*")
        list(APPEND app_sources ${src})
    elseif(src MATCHES ".*/Core/Src/.*")
        list(APPEND app_sources ${src})
    elseif(src MATCHES ".*/Startup/startup_stm32f407vgtx.s")
        list(APPEND app_sources ${src})
    elseif(src MATCHES ".*/Drivers/stm32f4xx-hal-driver/Src/.*")
        list(APPEND app_sources ${src})
    endif()
endforeach()

message(STATUS "Building target ${APP_TARGET} with sources:")
foreach(file IN LISTS app_sources)
    message(STATUS "+ ${file}")
endforeach()

# Create executable first
add_executable(${APP_TARGET}
    ${app_sources}
)

# Set output directory for ELF, BIN, and HEX files
set_target_properties(${APP_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR_BIN}
)

# Include paths
target_include_directories(${APP_TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/Drivers//stm32f4xx-hal-driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/cmsis-device-f4/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Core/Include
	${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/App/Inc
)

# Compiler defines
target_compile_definitions(${APP_TARGET} PRIVATE
    STM32F407xx
    USE_HAL_DRIVER
    USE_FULL_LL_DRIVER
)

# Linker script (set externally from parent CMakeLists)
target_link_options(${APP_TARGET} PRIVATE
    -T${app_linker_script_SRC}
    ${cpu_PARAMS}
    ${linker_OPTS}
    -Wl,--gc-sections -Wl,-Map=${CMAKE_BINARY_DIR_BIN}/${APP_TARGET}.map
    --specs=nosys.specs
    -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
    -Wl,-z,max-page-size=8
    -Wl,--print-memory-usage
    -u _printf_float
)

# Compiler options
target_compile_options(${APP_TARGET} PRIVATE
    ${cpu_PARAMS}
    ${compiler_OPTS}
    -Wall -Wextra -Wpedantic -Wno-unused-parameter
    $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -MMD -MP>
    $<$<CONFIG:Debug>:-Og -g3 -ggdb>
    $<$<CONFIG:Release>:-Og -g0>
)

# Post-build: generate HEX and BIN
add_custom_command(TARGET ${APP_TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${APP_TARGET}> ${CMAKE_BINARY_DIR_BIN}/${APP_TARGET}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${APP_TARGET}> ${CMAKE_BINARY_DIR_BIN}/${APP_TARGET}.bin
    COMMENT "Generating HEX and BIN for App"
)

# Print memory usage
add_custom_command(TARGET ${APP_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "******** Print size information:"
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${APP_TARGET}>
    COMMENT "Size information"
)

# Flash target
add_custom_target(app-flash
    DEPENDS ${APP_TARGET}
    COMMAND ${STM32CUBE_PROGRAMMER} -c port=SWD -d ${CMAKE_BINARY_DIR_BIN}/${APP_TARGET}.elf
    COMMENT "Flashing App via ST-Link"
)
