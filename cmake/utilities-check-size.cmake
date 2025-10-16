# =========================================================================
# cmake/utilities-check-size.cmake
# This script checks the sizes of the built bootloader and application,
# compares them against predefined limits, and outputs a summary table.
# If limits are exceeded, the build fails with an error.
# It also exports size data to a JSON file for CI dashboards.
# =========================================================================

# Policy CMP0007 is not set: list command no longer ignores empty elements.
# Ensure list() handles empty elements properly
cmake_policy(SET CMP0007 NEW)
# =========================================================================
# CMake Warning (dev) at /home/kojot/repos/ha-ctrl/cmake/utilities-check-size.cmake:16 (list):
#   Policy CMP0007 is not set: list command no longer ignores empty elements.
#   Run "cmake --help-policy CMP0007" for policy details.  Use the cmake_policy
#   command to set the policy and suppress this warning.  List has value =
#   [;80696;2040;2560;85296;14d30].
# This warning is for project developers.  Use -Wno-dev to suppress it.
# =========================================================================

# Helper function to parse size output files
# Expects a file path and a prefix for variable names (e.g., "BOOT" or "APP")
function(parse_size_file file prefix)
    file(READ "../bin${file}" CONTENT)
    string(REGEX MATCH "[ \t]*[0-9]+[ \t]+[0-9]+[ \t]+[0-9]+" LINE "${CONTENT}") # Match the line with sizes
    string(STRIP "${LINE}" LINE) # Remove leading/trailing whitespace
    string(REGEX REPLACE "[ \t]+" ";" VALUES "${LINE}") # Split the matched line into a list

# Extract individual sizes
    list(GET VALUES 0 TEXT)
    list(GET VALUES 1 DATA)
    list(GET VALUES 2 BSS)
    set(${prefix}_TEXT ${TEXT} PARENT_SCOPE)
    set(${prefix}_DATA ${DATA} PARENT_SCOPE)
    set(${prefix}_BSS ${BSS} PARENT_SCOPE)
endfunction()

# Parse bootloader and app size files
set(BOOT_SIZE_FILE "${CMAKE_BINARY_DIR_BIN}/ha-ctrl-boot_size.txt")
set(APP_SIZE_FILE  "${CMAKE_BINARY_DIR_BIN}/ha-ctrl-app_size.txt")

parse_size_file("${BOOT_SIZE_FILE}" BOOT)
parse_size_file("${APP_SIZE_FILE}" APP)

# Compute totals
math(EXPR FLASH_TOTAL "${BOOT_TEXT} + ${BOOT_DATA} + ${APP_TEXT} + ${APP_DATA}")
math(EXPR RAM_TOTAL   "${BOOT_DATA} + ${BOOT_BSS} + ${APP_DATA} + ${APP_BSS}")

# Define limits (in bytes)
# STM32F407VG has: (1048576) 1 MB FLASH, (196608) 192 KB RAM
# We set limits to 90% of total to leave some headroom:
# - FLASH: 917504 bytes (896 KB)
# - RAM:   131072 bytes (128 KB)
# This allows for a maximum usage of:
#     FLASH: 1 MB (1024 KB)
#     RAM: 192 KB (split between SRAM and CCMRAM)
set(FLASH_LIMIT_BYTES 917504) # 896 KB
set(RAM_LIMIT_BYTES   131072) # 128 KB

math(EXPR FLASH_USAGE_PERCENT "(${FLASH_TOTAL} * 100) / ${FLASH_LIMIT_BYTES}")
math(EXPR RAM_USAGE_PERCENT   "(${RAM_TOTAL} * 100) / ${RAM_LIMIT_BYTES}")


# Print formatted summary table
message(STATUS "")
message(STATUS "┌────────────────────┬──────────────┬────────────────────────────────────────┐")
message(STATUS "│ Section            │ Size (bytes) │ Description                            │")
message(STATUS "├────────────────────┼──────────────┼────────────────────────────────────────┤")
message(STATUS "│ Bootloader .text   │ ${BOOT_TEXT}        │ Code (in FLASH)                        │")
message(STATUS "│ Bootloader .data   │ ${BOOT_DATA}         │ Initialized data (RAM, stored in FLASH)│")
message(STATUS "│ Bootloader .bss    │ ${BOOT_BSS}         │ Uninitialized data (RAM)              │")
message(STATUS "│ App .text          │ ${APP_TEXT}        │ Code (in FLASH)                        │")
message(STATUS "│ App .data          │ ${APP_DATA}         │ Initialized data (RAM, stored in FLASH)│")
message(STATUS "│ App .bss           │ ${APP_BSS}         │ Uninitialized data (RAM)              │")
message(STATUS "├────────────────────┼──────────────┼────────────────────────────────────────┤")
message(STATUS "│ FLASH total        │ ${FLASH_TOTAL}        │ .text + .data                         │")
message(STATUS "│ RAM total          │ ${RAM_TOTAL}         │ .data + .bss                          │")
message(STATUS "└────────────────────┴──────────────┴────────────────────────────────────────┘")
message(STATUS "")
message(STATUS "FLASH usage: ${FLASH_TOTAL} bytes (${FLASH_USAGE_PERCENT}%)")
message(STATUS "RAM usage: ${RAM_TOTAL} bytes (${RAM_USAGE_PERCENT}%)")

# Enforce limits
if(FLASH_USAGE_PERCENT GREATER 90)
    message(FATAL_ERROR "FLASH usage exceeds 90% (${FLASH_USAGE_PERCENT}%) — build failed.")
endif()

if(RAM_USAGE_PERCENT GREATER 90)
    message(FATAL_ERROR "RAM usage exceeds 90% (${RAM_USAGE_PERCENT}%) — build failed.")
endif()

# Export to JSON for CI dashboards
set(JSON "{\n")
string(APPEND JSON "  \"bootloader\": {\n    \"text\": ${BOOT_TEXT},\n    \"data\": ${BOOT_DATA},\n    \"bss\": ${BOOT_BSS}\n  },\n")
string(APPEND JSON "  \"application\": {\n    \"text\": ${APP_TEXT},\n    \"data\": ${APP_DATA},\n    \"bss\": ${APP_BSS}\n  },\n")
string(APPEND JSON "  \"combined\": {\n    \"flash\": ${FLASH_TOTAL},\n    \"ram\": ${RAM_TOTAL},\n    \"flash_usage_percent\": ${FLASH_USAGE_PERCENT},\n    \"ram_usage_percent\": ${RAM_USAGE_PERCENT}\n  }\n")
string(APPEND JSON "}\n")

# Define output JSON file path (relative to bin directory)
set(JSON_COMBINED_FILE "../bin/combined_size.json")
file(WRITE "${JSON_COMBINED_FILE}" "${JSON}")
message(STATUS "Size data written to: ${JSON_COMBINED_FILE}")



