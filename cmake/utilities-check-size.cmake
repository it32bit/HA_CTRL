# =========================================================================
# cmake/utilities-check-size.cmake
# =========================================================================
# Size Check Script (invoked via -P)
# When use -P, CMake executes the script in script mode,
# which does not set CMAKE_CURRENT_LIST_DIR, and CMake is not in a project context.
# So it can't resolve relative paths like cmake/, but below is workaround.
# CMAKE_CURRENT_LIST_DIR is a built-in variable in CMake that gives the directory of the current script being processed.
# =========================================================================
# Load MCU traits and size report utilities
include("${CMAKE_CURRENT_LIST_DIR}/platform-traits.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/size-report.cmake")
# =========================================================================
# This script checks the sizes of the built bootloader and application,
# compares them against predefined limits, and outputs a summary table.
# If limits are exceeded, the build fails with an error.
# It also exports size data to a JSON file for CI dashboards.
# =========================================================================
# Policy CMP0007 is not set: list command no longer ignores empty elements.
# Ensure list() handles empty elements properly
# =========================================================================
# CMake Warning (dev) at /home/kojot/repos/ha-ctrl/cmake/utilities-check-size.cmake:16 (list):
#   Policy CMP0007 is not set: list command no longer ignores empty elements.
#   Run "cmake --help-policy CMP0007" for policy details.  Use the cmake_policy
#   command to set the policy and suppress this warning.  List has value =
#   [;80696;2040;2560;85296;14d30].
# This warning is for project developers.  Use -Wno-dev to suppress it.
# =========================================================================
cmake_policy(SET CMP0007 NEW)

# Input files
set(BPRIM_SIZE_FILE "${SCRIPT_OUTPUT_DIR}/ha-ctrl-prim_size.txt")
set(BSEC_SIZE_FILE  "${SCRIPT_OUTPUT_DIR}/ha-ctrl-sec_size.txt")
set(APP_SIZE_FILE   "${SCRIPT_OUTPUT_DIR}/ha-ctrl-app_size.txt")

# Parse size files
parse_size_file("${BPRIM_SIZE_FILE}" BPRIM)
parse_size_file("${BSEC_SIZE_FILE}"  BSEC)
parse_size_file("${APP_SIZE_FILE}"   APP)

# Compute totals and percentages
compute_size_totals()

# Print summary and enforce limits
print_size_summary()

# Export to JSON
set(JSON_OUTPUT_DIR "${SCRIPT_OUTPUT_DIR}")
write_size_json("${JSON_OUTPUT_DIR}/combined_size.json")
