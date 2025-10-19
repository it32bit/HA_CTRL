# =========================================================================
# cmake/utilities.cmake
# Utility functions to enhance the CMake build process.
# =========================================================================

# Function to print size information after building a target
function(add_size_print target)
    set(SIZE_OUTPUT_FILE "${CMAKE_BINARY_DIR_BIN}/${target}_size.txt")

    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "******** Size Summary for ${target}:" >> ${SIZE_OUTPUT_FILE}
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}> >> ${SIZE_OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E echo "******** Detailed Memory Usage:"
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
    )
endfunction()

# Function to create combined binary: Bootloader + Padding + App
function(add_combined_binary target_boot target_app output_name)
    set(BOOT_ELF $<TARGET_FILE:${target_boot}>)
    set(APP_ELF  $<TARGET_FILE:${target_app}>)

    set(BOOT_BIN "${CMAKE_BINARY_DIR_BIN}/${target_boot}.bin")
    set(APP_BIN  "${CMAKE_BINARY_DIR_BIN}/${target_app}.bin")
    set(PAD_BIN  "${CMAKE_BINARY_DIR_BIN}/pad.bin")
    set(COMBINED_BIN "${CMAKE_BINARY_DIR_BIN}/${output_name}.bin")

    add_custom_command(
        OUTPUT ${COMBINED_BIN}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${BOOT_ELF} ${BOOT_BIN}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${APP_ELF} ${APP_BIN}
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/combine_binaries.sh
                ${APP_ELF} ${BOOT_ELF} ${BOOT_BIN} ${APP_BIN} ${PAD_BIN} ${COMBINED_BIN}
        DEPENDS ${target_boot} ${target_app}
        COMMENT "Creating combined binary using combine_binaries.sh"
    )

    add_custom_target(${output_name}_combined ALL
        DEPENDS ${COMBINED_BIN}
    )
endfunction()

# Function to extract firmware version from ELF and create a versioned zip in _firmware/
function(add_firmware_packaging target_app target_boot project_name)
    set(APP_VERSION_TXT "${CMAKE_BINARY_DIR}/firmware_version_app.txt")
    set(BOOT_VERSION_TXT "${CMAKE_BINARY_DIR}/firmware_version_boot.txt")

    set(APP_ELF $<TARGET_FILE:${target_app}>)
    set(BOOT_ELF $<TARGET_FILE:${target_boot}>)

    # Extract version from app ELF
    add_custom_command(
        OUTPUT ${APP_VERSION_TXT}
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/extract_version.sh ${APP_ELF} > ${APP_VERSION_TXT}
        DEPENDS ${target_app}
        COMMENT "Extracting firmware version from app ELF"
    )

    # Extract version from boot ELF
    add_custom_command(
        OUTPUT ${BOOT_VERSION_TXT}
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/extract_version.sh ${BOOT_ELF} > ${BOOT_VERSION_TXT}
        DEPENDS ${target_boot}
        COMMENT "Extracting firmware version from boot ELF"
    )

    add_custom_target(extract_versions ALL
        DEPENDS ${APP_VERSION_TXT} ${BOOT_VERSION_TXT}
    )

    # Create zip with both versions in name
add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/_firmware/${project_name}_firmware.zip
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/_firmware
    COMMAND ${CMAKE_SOURCE_DIR}/cmake/package_firmware.sh
            ${APP_VERSION_TXT}
            ${BOOT_VERSION_TXT}
            ${CMAKE_BINARY_DIR_BIN}
            ${CMAKE_SOURCE_DIR}/_firmware
            ${project_name}
    DEPENDS extract_versions ${CMAKE_BINARY_DIR_BIN}/${project_name}_combined_image.bin
    COMMENT "Packaging bin folder into versioned zip and moving to _firmware"
)

add_custom_target(package_firmware ALL
    DEPENDS ${CMAKE_SOURCE_DIR}/_firmware/${project_name}_firmware.zip
)
endfunction()
