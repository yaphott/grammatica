# Grammatica C Library Makefile
# This Makefile provides convenient shortcuts for building, testing, and managing the C library

.PHONY: all configure build test clean rebuild run-tests install help

# Default target
all: build

# Configuration variables
BUILD_DIR := build
CMAKE := cmake
CMAKE_BUILD_TYPE ?= Release
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

help:
	@echo "Grammatica C Library - Available targets:"
	@echo ""
	@echo "  make configure         - Configure the build system with CMake"
	@echo "  make build            - Build the library and tests"
	@echo "  make test             - Run all tests"
	@echo "  make test-verbose     - Run tests with verbose output"
	@echo "  make clean            - Remove build artifacts"
	@echo "  make rebuild          - Clean and rebuild everything"
	@echo "  make run-tests        - Alias for 'test'"
	@echo "  make debug            - Configure and build in Debug mode"
	@echo "  make release          - Configure and build in Release mode (default)"
	@echo "  make install          - Install the library (requires sudo)"
	@echo "  make format           - Format C/C++ code with clang-format"
	@echo "  make help             - Show this help message"
	@echo ""
	@echo "Environment variables:"
	@echo "  CMAKE_BUILD_TYPE      - Build type (Debug or Release, default: Release)"
	@echo ""

# Configure the build with CMake
configure:
	@echo "Configuring build system..."
	@mkdir -p $(BUILD_DIR)
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
	@echo "Configuration complete."

# Build the project
build: configure
	@echo "Building project..."
	$(CMAKE) --build $(BUILD_DIR) -j$(NPROC)
	@echo "Build complete."

# Run tests
test: build
	@echo "Running tests..."
	cd $(BUILD_DIR) && ctest --output-on-failure
	@echo "Tests complete."

# Run tests with verbose output
test-verbose: build
	@echo "Running tests (verbose)..."
	cd $(BUILD_DIR) && ctest --verbose
	@echo "Tests complete."

# Run tests and show only summary
test-quiet: build
	@echo "Running tests (quiet)..."
	cd $(BUILD_DIR) && ctest --output-on-failure --quiet
	@echo "Tests complete."

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete."

# Rebuild everything from scratch
rebuild: clean build

# Alias for test
run-tests: test

# Debug build
debug:
	@echo "Configuring for Debug build..."
	@$(MAKE) CMAKE_BUILD_TYPE=Debug build

# Release build (default)
release:
	@echo "Configuring for Release build..."
	@$(MAKE) CMAKE_BUILD_TYPE=Release build

# Install the library (requires appropriate permissions)
install: build
	@echo "Installing library..."
	cd $(BUILD_DIR) && $(CMAKE) --install .
	@echo "Installation complete."

# Format code with clang-format
format:
	@echo "Formatting C/C++ code..."
	@if command -v clang-format >/dev/null 2>&1; then \
		find src include tests -type f \( -name "*.c" -o -name "*.h" -o -name "*.cpp" \) -exec clang-format -i {} \; ; \
		echo "Formatting complete."; \
	else \
		echo "clang-format not found. Please install it to format code."; \
		exit 1; \
	fi

# Quick test (build and run tests in one command)
quick: 
	@$(MAKE) build test
