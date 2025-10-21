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
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${target}> >> ${SIZE_OUTPUT_FILE}
        COMMAND ${CMAKE_COMMAND} -E echo "******** Detailed Memory Usage:"
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${target}>
    )

endfunction()

# Function to create combined binary: Bootloader + Padding + App
function(add_combined_binary target_bprim target_bsec target_app output_name)
    # Get ELF file paths from targets
    set(BOOT_PRIM_ELF $<TARGET_FILE:${target_bprim}>)
    set(BOOT_SEC_ELF  $<TARGET_FILE:${target_bsec}>)
    set(APP_ELF       $<TARGET_FILE:${target_app}>)

    # Define output binary file paths
    set(BOOT_PRIM_BIN "${CMAKE_BINARY_DIR_BIN}/${target_bprim}.bin")
    set(BOOT_SEC_BIN  "${CMAKE_BINARY_DIR_BIN}/${target_bsec}.bin")
    set(APP_BIN       "${CMAKE_BINARY_DIR_BIN}/${target_app}.bin")
    set(COMBINED_BIN  "${CMAKE_BINARY_DIR_BIN}/${output_name}.bin")

    # Custom command to generate binaries and combine
    add_custom_command(
        OUTPUT ${COMBINED_BIN}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${BOOT_PRIM_ELF} ${BOOT_PRIM_BIN}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${BOOT_SEC_ELF}  ${BOOT_SEC_BIN}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${APP_ELF}       ${APP_BIN}
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/combine_binaries.sh
                ${APP_ELF} ${BOOT_SEC_ELF} ${BOOT_PRIM_ELF}
                ${APP_BIN} ${BOOT_SEC_BIN} ${BOOT_PRIM_BIN}
                ${COMBINED_BIN}
        DEPENDS ${target_bprim} ${target_bsec} ${target_app}
        COMMENT "Combining firmware: BootPrim + BootSec + App into ${output_name}.bin"
    )

    # Add custom target that depends on the combined binary
    add_custom_target(${output_name}_combined ALL
        DEPENDS ${COMBINED_BIN}
    )
endfunction()

# Function to extract firmware versions from ELF files and create a versioned zip
function(add_firmware_packaging target_app target_bsec target_bprim project_name)
    # Paths to output version text files
    set(APP_VERSION_TXT  "${CMAKE_BINARY_DIR}/firmware_version_app.txt")
    set(BOOT_VERSION_TXT "${CMAKE_BINARY_DIR}/firmware_version_boot.txt")

    # ELF files from CMake targets
    set(APP_ELF      $<TARGET_FILE:${target_app}>)
    set(BOOT_SEC_ELF $<TARGET_FILE:${target_bsec}>)
    set(BOOT_PRIM_ELF $<TARGET_FILE:${target_bprim}>)

    # Extract app version from APP_ELF
    add_custom_command(
        OUTPUT ${APP_VERSION_TXT}
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/extract_version.sh ${APP_ELF} > ${APP_VERSION_TXT}
        DEPENDS ${target_app}
        COMMENT "Extracting firmware version from application ELF"
    )

    # Extract boot version (BootPrim + BootSec, optional merge logic)
    add_custom_command(
        OUTPUT ${BOOT_VERSION_TXT}
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/extract_version.sh ${BOOT_PRIM_ELF} > ${BOOT_VERSION_TXT}
        DEPENDS ${target_bprim}
        COMMENT "Extracting firmware version from BootPrim ELF"
    )

    # Target to ensure version files are built
    add_custom_target(extract_versions ALL
        DEPENDS ${APP_VERSION_TXT} ${BOOT_VERSION_TXT}
    )

    # Output zip location
    set(OUTPUT_ZIP "${CMAKE_SOURCE_DIR}/_firmware/${project_name}_firmware.zip")

    # Create firmware zip with versioned name
    add_custom_command(
        OUTPUT ${OUTPUT_ZIP}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/_firmware
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/package_firmware.sh
                ${APP_VERSION_TXT}
                ${BOOT_VERSION_TXT}
                ${CMAKE_BINARY_DIR_BIN}
                ${CMAKE_SOURCE_DIR}/_firmware
                ${project_name}
        DEPENDS extract_versions ${CMAKE_BINARY_DIR_BIN}/${project_name}_combined_image.bin
        COMMENT "Packaging combined firmware into versioned zip: ${project_name}_firmware.zip"
    )

    # Target to package the firmware
    add_custom_target(package_firmware ALL
        DEPENDS ${OUTPUT_ZIP}
    )
endfunction()
