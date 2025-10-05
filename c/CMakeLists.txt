cmake_minimum_required(VERSION 3.18)
project(grammatica C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wno-unused-parameter
        -fPIC
    )
endif()

# Thread support
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Source files
set(GRAMMATICA_SOURCES
    src/common.c
    src/utils.c
    src/constants.c
    src/string_builder.c
    src/grammar/base.c
    src/grammar/string.c
    src/grammar/char_range.c
    src/grammar/derivation_rule.c
    src/grammar/grammar.c
)

# Create static library
add_library(grammatica_static STATIC ${GRAMMATICA_SOURCES})
target_link_libraries(grammatica_static PRIVATE Threads::Threads)
set_target_properties(grammatica_static PROPERTIES OUTPUT_NAME grammatica)

# Create shared library
add_library(grammatica_shared SHARED ${GRAMMATICA_SOURCES})
target_link_libraries(grammatica_shared PRIVATE Threads::Threads)
set_target_properties(grammatica_shared PROPERTIES OUTPUT_NAME grammatica)

# Testing
enable_testing()

# Add googletest subdirectory
add_subdirectory(googletest)
include_directories(googletest/googletest/include)
include(GoogleTest)

# Test files
set(TEST_SOURCES
    tests/test_utils.cpp
    tests/test_constants.cpp
    tests/test_string_builder.cpp
    tests/grammar/test_base.cpp
    tests/grammar/test_string.cpp
    tests/grammar/test_char_range.cpp
    tests/grammar/test_derivation_rule.cpp
    tests/grammar/test_grammar.cpp
)

# Create test executable
add_executable(grammatica_tests ${TEST_SOURCES})
target_compile_options(grammatica_tests PRIVATE -Wno-sign-compare)
target_link_libraries(grammatica_tests
    grammatica_static
    gtest_main
)

# Discover tests automatically
gtest_discover_tests(grammatica_tests)

# Example executable
add_executable(grammatica_example examples/example.c)
target_link_libraries(grammatica_example grammatica_static)

# Installation
install(TARGETS grammatica_static grammatica_shared
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY include/grammatica DESTINATION include)
