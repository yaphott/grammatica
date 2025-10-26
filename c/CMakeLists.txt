cmake_minimum_required(VERSION 3.18)
project(grammatica C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Thread support
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Library source files
set(GRAMMATICA_SOURCES
    src/constants.c
    src/utils.c
    src/grammar_base.c
    src/char_range.c
    src/string.c
    src/derivation_rule.c
    src/group_base.c
    src/and.c
    src/or.c
)

# Build library
add_library(grammatica STATIC ${GRAMMATICA_SOURCES})
target_link_libraries(grammatica PRIVATE Threads::Threads)

# GoogleTest setup
enable_testing()
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Use local googletest directory
add_subdirectory(googletest)

# Test source files
set(TEST_SOURCES
    tests/test_constants.cpp
    tests/test_utils.cpp
    tests/test_grammar_base.cpp
    tests/test_char_range.cpp
    tests/test_string.cpp
    tests/test_derivation_rule.cpp
    tests/test_group_base.cpp
    tests/test_and.cpp
    tests/test_or.cpp
)

# Build tests
add_executable(grammatica_tests ${TEST_SOURCES})
target_link_libraries(grammatica_tests
    PRIVATE
    grammatica
    GTest::gtest_main
    Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(grammatica_tests)
