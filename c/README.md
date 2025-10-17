# Grammatica C Library - Quick Start Guide

This directory contains the C implementation of the Grammatica library.

## Building and Testing

We provide a Makefile for convenient building and testing. Here are the most common commands:

### Quick Start

```bash
# Build and run tests (most common)
make test

# Or use the shorthand
make
```

### Common Commands

```bash
# Show all available commands
make help

# Configure the build system
make configure

# Build the library and tests
make build

# Run all tests
make test

# Run tests with verbose output
make test-verbose

# Clean build artifacts
make clean

# Clean and rebuild everything
make rebuild
```

### Build Types

```bash
# Build in Debug mode (with debug symbols)
make debug

# Build in Release mode (optimized, default)
make release
```

### Code Formatting

```bash
# Format all C/C++ code with clang-format
make format
```

## Project Structure

```
c/
├── CMakeLists.txt          # CMake build configuration
├── Makefile                # Convenient build shortcuts
├── include/                # Public header files
│   ├── grammatica.h
│   ├── grammatica_constants.h
│   ├── grammatica_group.h
│   ├── grammatica_internal.h
│   └── grammatica_utils.h
├── src/                    # Implementation files
│   ├── and.c
│   ├── char_range.c
│   ├── constants.c
│   ├── derivation_rule.c
│   ├── grammar_base.c
│   ├── group_base.c
│   ├── or.c
│   ├── string.c
│   └── utils.c
├── tests/                  # Test files (using GoogleTest)
│   ├── test_and.cpp
│   ├── test_char_range.cpp
│   ├── test_constants.cpp
│   ├── test_derivation_rule.cpp
│   ├── test_grammar_base.cpp
│   ├── test_group_base.cpp
│   ├── test_or.cpp
│   ├── test_string.cpp
│   └── test_utils.cpp
└── googletest/             # Local GoogleTest library
```

## Manual CMake Usage

If you prefer to use CMake directly:

```bash
# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Test
cd build && ctest --output-on-failure
```

## Requirements

- CMake 3.18 or later
- A C17 compiler (GCC, Clang, etc.)
- A C++17 compiler (for tests)
- GoogleTest (included locally)

## Installation

```bash
# Build and install the library (may require sudo)
make install
```

## Tips

- Use `make test-quiet` for a cleaner test output
- Use `make rebuild` when you want to ensure a clean build
- The Makefile automatically detects the number of CPU cores for parallel builds
- All build artifacts go into the `build/` directory
