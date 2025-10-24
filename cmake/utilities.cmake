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

# =========================================================================
# Metadata Extraction and Conversion
# =========================================================================

# Extract app version from ELF
set(APP_VERSION_TXT  "${CMAKE_BINARY_DIR}/firmware_version_app.txt")
add_custom_command(
    OUTPUT ${APP_VERSION_TXT}
    COMMAND ${CMAKE_SOURCE_DIR}/cmake/extract_version.sh
            $<TARGET_FILE:ha-ctrl-app> > ${APP_VERSION_TXT}
    DEPENDS ha-ctrl-app
    COMMENT "Extracting firmware version from application ELF"
)

# Extract boot-sec version from ELF
set(BOOTSEC_VERSION_TXT  "${CMAKE_BINARY_DIR}/firmware_version_bootsec.txt")
add_custom_command(
    OUTPUT ${BOOTSEC_VERSION_TXT}
    COMMAND ${CMAKE_SOURCE_DIR}/cmake/extract_version.sh
            $<TARGET_FILE:ha-ctrl-sec> > ${BOOTSEC_VERSION_TXT}
    DEPENDS ha-ctrl-sec
    COMMENT "Extracting firmware version from BootSec ELF"
)

# Metadata Extraction
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR_BIN}/app_metadata.bin
    COMMAND python3 ${CMAKE_SOURCE_DIR}/Tools/gen_metadata.py
            ${APP_VERSION_TXT}
            ${CMAKE_BINARY_DIR_BIN}/ha-ctrl-app.bin
            ${CMAKE_BINARY_DIR_BIN}/app_metadata.bin
    DEPENDS ${APP_VERSION_TXT}
    COMMENT "Generating app metadata"
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR_BIN}/bootsec_metadata.bin
    COMMAND python3 ${CMAKE_SOURCE_DIR}/Tools/gen_metadata.py
            ${BOOTSEC_VERSION_TXT}
            ${CMAKE_BINARY_DIR_BIN}/ha-ctrl-sec.bin
            ${CMAKE_BINARY_DIR_BIN}/bootsec_metadata.bin
    DEPENDS ${BOOTSEC_VERSION_TXT}
    COMMENT "Generating boot-sec metadata"
)

if(NOT TARGET generate_metadata)
    add_custom_target(generate_metadata ALL
        DEPENDS ${CMAKE_BINARY_DIR_BIN}/app_metadata.bin
                ${CMAKE_BINARY_DIR_BIN}/bootsec_metadata.bin
    )
endif()

# =========================================================================
# Combined Binary Creation
# =========================================================================

function(add_combined_firmware_with_metadata target_app target_bsec app_meta_bin sec_meta_bin output_name)
    set(APP_ELF       $<TARGET_FILE:${target_app}>)
    set(BOOT_SEC_ELF  $<TARGET_FILE:${target_bsec}>)

    set(APP_BIN       "${CMAKE_BINARY_DIR_BIN}/${target_app}.bin")
    set(BOOT_SEC_BIN  "${CMAKE_BINARY_DIR_BIN}/${target_bsec}.bin")
    set(APP_META_BIN  "${CMAKE_BINARY_DIR_BIN}/${app_meta_bin}")
    set(SEC_META_BIN  "${CMAKE_BINARY_DIR_BIN}/${sec_meta_bin}")
    set(COMBINED_BIN  "${CMAKE_BINARY_DIR_BIN}/${output_name}.bin")

    add_custom_command(
        OUTPUT ${COMBINED_BIN}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${APP_ELF}      ${APP_BIN}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${BOOT_SEC_ELF} ${BOOT_SEC_BIN}
        COMMAND ${CMAKE_COMMAND} -E echo "Combining firmware with metadata..."
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR}/cmake
                python3 ${CMAKE_SOURCE_DIR}/cmake/combine_firmware_update_bin.py
                ${APP_BIN}
                ${APP_META_BIN}
                ${BOOT_SEC_BIN}
                ${SEC_META_BIN}
                ${COMBINED_BIN}
        DEPENDS generate_metadata ${target_app} ${target_bsec}
        COMMENT "Creating combined firmware binary with metadata: ${output_name}.bin"
    )

    add_custom_target(${output_name}_combined ALL
        DEPENDS ${COMBINED_BIN}
    )
endfunction()

# =========================================================================
# Firmware Packaging
# =========================================================================

function(add_firmware_packaging target_app target_bsec target_bprim project_name)
    set(APP_VERSION_TXT  "${CMAKE_BINARY_DIR_BIN}/firmware_version_app.txt")
    set(BOOT_VERSION_TXT "${CMAKE_BINARY_DIR_BIN}/firmware_version_boot.txt")

    set(APP_ELF      $<TARGET_FILE:${target_app}>)
    set(BOOT_SEC_ELF $<TARGET_FILE:${target_bsec}>)
    set(BOOT_PRIM_ELF $<TARGET_FILE:${target_bprim}>)

    add_custom_command(
        OUTPUT ${APP_VERSION_TXT}
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/extract_version.sh ${APP_ELF} > ${APP_VERSION_TXT}
        DEPENDS ${target_app}
        COMMENT "Extracting firmware version from application ELF"
    )

    add_custom_command(
        OUTPUT ${BOOT_VERSION_TXT}
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/extract_version.sh ${BOOT_PRIM_ELF} > ${BOOT_VERSION_TXT}
        DEPENDS ${target_bprim}
        COMMENT "Extracting firmware version from BootPrim ELF"
    )

    add_custom_target(extract_versions ALL
        DEPENDS ${APP_VERSION_TXT} ${BOOT_VERSION_TXT}
    )

    set(OUTPUT_ZIP "${CMAKE_SOURCE_DIR}/_firmware/${project_name}_firmware.zip")

    add_custom_command(
        OUTPUT ${OUTPUT_ZIP}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/_firmware
        COMMAND ${CMAKE_SOURCE_DIR}/cmake/package_firmware.sh
                ${APP_VERSION_TXT}
                ${BOOT_VERSION_TXT}
                ${CMAKE_BINARY_DIR_BIN}
                ${CMAKE_SOURCE_DIR}/_firmware
                ${project_name}
        DEPENDS extract_versions ${CMAKE_BINARY_DIR_BIN}/${project_name}_combined_update_image.bin

        COMMENT "Packaging combined firmware into versioned zip: ${project_name}_firmware.zip"
    )

    add_custom_target(package_firmware ALL
        DEPENDS ${OUTPUT_ZIP}
    )
endfunction()
