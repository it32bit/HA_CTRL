
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

# Unit test executable
add_executable(run_tests
    main.cpp
    test_hal_adc.cpp
    hal_adc_mock.cpp
)

# Link with CppUTest
target_link_libraries(run_tests
    CppUTest
    CppUTestExt
)

# Include your App headers for testing
target_include_directories(run_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/App/Inc
)

# Compile with C++ flags
target_compile_features(run_tests PRIVATE cxx_std_17)
