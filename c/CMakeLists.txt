cmake_minimum_required(VERSION 3.18)
project(grammatica C CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_library(grammatica_static STATIC
    src/common.c
    src/constants.c
    src/string_builder.c
    src/utils.c
    src/grammar/base.c
    src/grammar/char_range.c
    src/grammar/derivation_rule.c
    src/grammar/grammar.c
    src/grammar/string.c
)
target_link_libraries(grammatica_static PRIVATE Threads::Threads)
set_target_properties(grammatica_static PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(grammatica_static PROPERTIES OUTPUT_NAME grammatica)

file(GLOB_RECURSE C_FILES CONFIGURE_DEPENDS src/*.c src/**/*.c)
set_source_files_properties(${C_FILES} PROPERTIES LANGUAGE CXX)
get_source_file_property(thing ./src/grammatica.c LANGUAGE)
target_compile_options(grammatica_static PUBLIC -xc++ -O3)
target_include_directories(grammatica_static PUBLIC src)

enable_testing()
add_subdirectory(googletest)
include(GoogleTest)
add_executable(main_test
    src/constants_test.c
    src/string_builder_test.c
    src/utils_test.c
    src/grammar/base_test.c
    src/grammar/char_range_test.c
    src/grammar/derivation_rule_test.c
    src/grammar/grammar_test.c
    src/grammar/string_test.c
)
target_link_libraries(
    main_test
    GTest::gtest_main
    grammatica_static
)
gtest_discover_tests(main_test)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.11.0")
    file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp *.h)
else()
    file(GLOB SOURCE_FILES *.cpp *.h */*.h */*.cpp)
endif()
