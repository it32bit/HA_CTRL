# =========================================================================
# cmake/compiler-warnings.cmake
# Function to Set Compiler Warning Flags
# =========================================================================
# This function sets common compiler warning flags for a given target.
# It supports GCC, Clang, and MSVC compilers.
# Usage:
#   set_project_warnings(target_name)
# Example:
#   set_project_warnings(APP_TARGET)
#   set_project_warnings(BOOT_TARGET)
# =========================================================================
function(set_project_warnings target)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target} PRIVATE
            -Wall # Enable all common warnings
            -Wextra # Enable extra warnings not covered by -Wall e.g., unused parameters
            -Wpedantic # Enforce strict ISO compliance
            -Wno-unused-parameter # Disable unused parameter warnings
            # -Wc++11-compat # Warn about constructs that are not compatible with C++11
            # -Wc++14-compat # Warn about constructs that are not compatible with C++14
            # -Wc++17-compat # Warn about constructs that are not compatible with C++17
            # -Wc++20-compat # Warn about constructs that are not compatible with C++20
            # -Wc++23-compat # Warn about constructs that are not compatible with C++23
            # -Wstack-protector # Warn about functions that are not protected by stack canaries
            # -Wunreachable-code # Warn if the compiler detects that code will never be executed
            # -Wundef # Warn if an undefined identifier is evaluated in an #if directive
            # -Wshadow # Warn if a variable declaration shadows one from a parent context
            # -Wshadow=local # Warn if a local variable shadows another local variable or parameter
            # -Wconversion # Warn on type conversions that may alter a value
            # -Wsign-conversion # Warn on sign conversions
            # -Wnull-dereference # Warn if a null dereference is detected
            # -Wdouble-promotion # Warn when float is promoted to double
            # -Wformat-nonliteral # Warn if the format string is not a string literal
            # -Wformat-security # Warn about possible security problems with format strings
            # -Wformat-y2k # Warn about potential year 2000 problems in format strings
            # -Wformat=2 # Enable format string warnings
            # -Wdeprecated # Warn about the use of deprecated features
            # -Wdeprecated-declarations # Warn about the use of deprecated declarations
            # -Winline # Warn if a function that is declared 'inline' cannot be inlined
            # -Wnon-virtual-dtor # Warn when a class with virtual functions has a non-virtual destructor
            # -Wmissing-declarations # Warn if a global function is defined without a previous declaration
            # -Wmissing-prototypes # Warn if a global function is defined without a previous prototype
            # -Wmissing-format-attribute # Warn if a function that takes a format string does not have the format attribute
            # -Wmissing-field-initializers # Warn if a struct or union is initialized with fewer fields than it has
            # -Wmissing-braces # Warn if an initializer list is missing braces
            # -Wold-style-cast # Warn on C-style casts
            # -Wuseless-cast # Warn if a cast does not change the type of an expression
            # -Woverloaded-virtual # Warn when a function declaration hides virtual functions from a base
            # -Wredundant-decls # Warn if anything is declared more than once in the same scope
            # -Wstrict-aliasing=2 # Warn about code that may break the strict aliasing rules
            # -Wstrict-overflow=5 # Warn about cases where the compiler assumes that signed overflow does not occur
            # -Wswitch-default # Warn whenever a switch statement does not have a default case
            # -Wswitch-enum # Warn whenever a switch statement covers all enum values
            # -Wthread-safety # Enable thread safety analysis
            # -Wunsafe-loop-optimizations # Warn if the compiler cannot prove that optimizations on loops will not change the program's behavior
            # -Wvector-operation-performance # Warn when a vector operation is performed in a way that is not optimal for performance
            # -Wvolatile-register-var # Warn if a variable that is declared 'volatile' is also declared 'register'
            # -Wzero-as-null-pointer-constant # Warn if the literal '0' is used as a null pointer constant
            # -Wliteral-suffix # Warn if a literal has a suffix that is not recognized
            # -Wfloat-conversion # Warn on implicit float to double conversions
            # -Wfloat-overflow # Warn if a floating point overflow occurs
            # -Wfloat-equal # Warn if floating point values are compared for equality
            # -Wpointer-arith # Warn about anything that depends on the "size of" a function type or void
            ## Language-specific flags
            $<$<COMPILE_LANGUAGE:ASM>:-x assembler-with-cpp -MMD -MP> # For assembly files
            $<$<CONFIG:Debug>:-Og -g3 -ggdb> # Debug infoging
            $<$<CONFIG:Release>:-Og -g0> # Optimize for size, no debug info
            # $<$<CONFIG:DEBUG>:-Werror>  # Treat warnings as errors in Debug
        )
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
            $<$<CONFIG:DEBUG>:/WX>  # Treat warnings as errors in Debug
        )
    endif()
endfunction()
