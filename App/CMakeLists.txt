cmake_minimum_required(VERSION 3.22)

include("${CMAKE_SOURCE_DIR}/cmake/platform-traits.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/project-sources.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/compiler-warnings.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/utilities.cmake")

# App target
set(APP_TARGET ha-ctrl-app)

# Linker script
set(app_linker_script_SRC ${app_linker_script_SRC}
    ${CMAKE_SOURCE_DIR}/App/app_linker.ld
)

# Filter only app-related sources
set(app_sources "")
foreach(src ${sources_SRCS})
    if(src MATCHES ".*/App/.*" OR
       src MATCHES ".*/Boot/Src/boot_flag_manager.cpp" OR
       src MATCHES ".*/Platform/STM32F4/Src/.*" OR
       src MATCHES ".*/Core/Src/.*" OR
       src MATCHES ".*/Startup/startup_stm32f407vgtx.s" OR
       src MATCHES ".*/Drivers/stm32f4xx-hal-driver/Src/.*")
        list(APPEND app_sources ${src})
    endif()
endforeach()

message(STATUS "Building target ${APP_TARGET} with sources:")
foreach(file IN LISTS app_sources)
    message(STATUS "[app] ${file}")
endforeach()

# Create executable first
add_executable(${APP_TARGET}
 ${app_sources}
)

# Link with Platform_STM32F4 HAL wrapper library
target_link_libraries(${APP_TARGET} PRIVATE
    Platform_STM32F4
)

# Set output directory for ELF, BIN, and HEX files
set_target_properties(${APP_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR_BIN}
)

# Include paths
target_include_directories(${APP_TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/App/Inc
	${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Platform/Interface
    ${CMAKE_SOURCE_DIR}/Boot/Inc
    ${MCU_INCLUDE_DIRS}
)

# Compiler defines
target_compile_definitions(${APP_TARGET} PRIVATE
    USE_HAL_DRIVER
    ${MCU_DEFINES}
)

# Linker script (set externally from parent CMakeLists)
target_link_options(${APP_TARGET} PRIVATE
    -T${app_linker_script_SRC}
    ${cpu_PARAMS}
    ${linker_OPTS}
    -Wl,-Map=${CMAKE_BINARY_DIR_BIN}/${APP_TARGET}.map
    -Wl,--gc-sections
    --specs=nosys.specs
    -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
    -Wl,-z,max-page-size=8
    -Wl,--print-memory-usage
    -u _printf_float
)

# CPU/Compiler specific flags and optimizations
target_compile_options(${APP_TARGET} PRIVATE
    ${cpu_PARAMS}
    ${compiler_OPTS}
)

# Set common warning flags, see cmake/compiler-warnings.cmake
set_project_warnings(${APP_TARGET})

# Post-build: generate HEX and BIN
add_custom_command(TARGET ${APP_TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${APP_TARGET}> ${CMAKE_BINARY_DIR_BIN}/${APP_TARGET}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${APP_TARGET}> ${CMAKE_BINARY_DIR_BIN}/${APP_TARGET}.bin
    COMMENT "Generating HEX and BIN for App"
)

# Print memory usage
add_size_print(${APP_TARGET})

# Flash target (using ST-Link via STM32CubeProgrammer)
add_custom_target(app-flash
    DEPENDS ${APP_TARGET}
    COMMAND ${STM32CUBE_PROGRAMMER} -c port=SWD -d ${CMAKE_BINARY_DIR_BIN}/${APP_TARGET}.elf
    COMMENT "Flashing App via ST-Link"
)
