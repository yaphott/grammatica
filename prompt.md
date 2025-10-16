# Grammatica C Rewrite: Complete Implementation Guide

## Project Overview

**Grammatica** is a GBNF (GGML BNF) grammar builder that exposes the GBNF notation system for restricting LLM outputs. The original Python implementation (located in [src/grammatica/](src/grammatica/)) has complete test coverage and is well-vetted. Your task is to perform a **complete rewrite** of both the Grammatica package and all its tests in C, creating a high-performance, thread-safe library.

## Primary Objectives

1. **Complete Package Rewrite**: Rewrite the entire Grammatica Python package from [src/grammatica/](src/grammatica/) into C, implementing all functionality in the [c/](c/) directory.

2. **Complete Test Suite Rewrite**: Rewrite all Python tests from [tests/](tests/) into C++ using GoogleTest, maintaining functional equivalence and test coverage.

3. **Performance and Thread Safety**: The C implementation must be significantly more performant than the Python version while being fully thread-safe.

4. **Architectural Compliance**: Follow all design patterns, conventions, and best practices specified in [requirements.md](requirements.md).

---

## Package Structure

### Python Source Files to Rewrite

The following Python modules must be completely rewritten in C:

**Core Modules:**

- `src/grammatica/__init__.py`
- `src/grammatica/constants.py`
- `src/grammatica/utils.py`
- `src/grammatica/_version.py`

**Grammar Modules:**

- `src/grammatica/grammar/__init__.py`
- `src/grammatica/grammar/base.py`
- `src/grammatica/grammar/char_range.py`
- `src/grammatica/grammar/derivation_rule.py`
- `src/grammatica/grammar/string.py`

**Group Modules:**

- `src/grammatica/grammar/group/__init__.py`
- `src/grammatica/grammar/group/base.py`
- `src/grammatica/grammar/group/and_.py`
- `src/grammatica/grammar/group/or_.py`

### Test Files to Rewrite

All test files must be rewritten in C++ using GoogleTest:

**Core Tests:**

- `tests/test_constants.py` → `c/tests/test_constants.cpp`
- `tests/test_utils.py` → `c/tests/test_utils.cpp`

**Grammar Tests:**

- `tests/grammar/test_base.py` → `c/tests/test_grammar_base.cpp`
- `tests/grammar/test_char_range.py` → `c/tests/test_char_range.cpp`
- `tests/grammar/test_derivation_rule.py` → `c/tests/test_derivation_rule.cpp`
- `tests/grammar/test_string.py` → `c/tests/test_string.cpp`

**Group Tests:**

- `tests/grammar/group/test_base.py` → `c/tests/test_group_base.cpp`
- `tests/grammar/group/test_and.py` → `c/tests/test_and.cpp`
- `tests/grammar/group/test_or.py` → `c/tests/test_or.cpp`

---

## Technical Requirements

### Build System and Language Standards

- **Build System**: Use **CMake 3.18** or higher
- **Language Standard**: Strict **C17** compliance for all implementation code
- **Test Framework**: **GoogleTest 1.17.0** for all unit tests
- **C++ Usage**: Minimize C++ code; use only when absolutely necessary (e.g., GoogleTest framework requirements)

### Thread Safety Requirements

- **Full Thread Safety**: All public API functions must be thread-safe
- **Context Isolation**: Use opaque context handles as described in [requirements.md](requirements.md)
- **Reentrant Design**: Implement reentrant functions with `_r` suffix pattern
- **No Global State**: Avoid shared global state; use thread-local contexts

### Memory Management

- **Stack Allocation Preferred**: Use stack allocation over heap allocation whenever possible
- **Explicit Ownership**: Clearly document ownership semantics for all functions
- **Paired Create/Destroy**: Every allocation function must have a corresponding deallocation function
- **Resource Cleanup**: Ensure proper cleanup on error paths

### Error Handling

- **Return Value Conventions**: Follow patterns from [requirements.md](requirements.md)
  - Pointers: Return `NULL` on error
  - Integers: Return `-1` or `0` on error
  - Predicates: Use special values (e.g., `2`) to indicate errors
- **Error Callbacks**: Implement error handler callbacks for detailed error reporting
- **Context-Based Errors**: Store error state in context objects

---

## Testing Requirements

### Test Coverage and Completeness

- **100% Functional Coverage**: All functionality from Python tests must be replicated
- **No Test Exclusions**: Do not exclude tests that seem trivial or redundant
- **Test Order Preservation**: Maintain the same test order as in Python test files
- **Parameterized Tests**: Use GoogleTest's `::testing::Values(...)` for parameterized tests (equivalent to Python's `pytest.mark.parametrize()`)

### Additional Testing Requirements

- **Coverage for C-Specific Code**: Additional tests for C-specific functionality (e.g., memory management, thread safety) are allowed but must be added **at the end** of each test file
- **Edge Case Testing**: Ensure all edge cases covered in Python tests are preserved
- **Error Path Testing**: Test all error conditions and cleanup paths

### Test File Organization

Each C++ test file should:

1. Mirror the structure of its corresponding Python test file
2. Use GoogleTest fixtures where Python tests use classes
3. Preserve test names (adapted to C++ naming conventions)
4. Maintain test execution order

---

## Coding Style and Conventions

### C Code Style

Follow the style guidelines from [requirements.md](requirements.md):

- **Idiomatic C**: Write clear, conventional C code
- **Return Types**: Place return type on the same line as the function name
- **Naming Conventions**:
  - Types: `TypeName_t` or `TypeName`
  - Functions: `lib_module_action()` pattern
  - Predicates: `lib_is_property()` pattern
  - Accessors: `lib_get_property()` pattern
  - Mutators: `lib_set_property()` pattern
- **Const Correctness**: Use `const` appropriately for input parameters

### API Design Patterns

Implement the following patterns from [requirements.md](requirements.md):

1. **Opaque Context Handles**: Hide implementation details using opaque types
2. **Context Management**: Provide `init()` and `finish()` functions for context lifecycle
3. **Ownership Documentation**: Clearly document which functions transfer ownership
4. **Reentrant Functions**: Provide `_r` suffix variants with explicit context parameters
5. **Callback Handlers**: Support error and notice callbacks

### Header Organization

Organize headers as follows:

- `c/include/grammatica.h`: Main public API
- `c/include/grammatica_constants.h`: Constants and enumerations
- `c/include/grammatica_utils.h`: Utility functions
- `c/include/grammatica_group.h`: Group-related functionality
- `c/include/grammatica_internal.h`: Internal/private definitions

---

## Implementation Workflow

### Step 1: Analyze Python Implementation

For each Python module:

1. Read and understand the complete implementation
2. Identify all classes, functions, and data structures
3. Map Python types to appropriate C types
4. Note any Python-specific features requiring C equivalents

### Step 2: Design C API

For each module:

1. Design opaque types for Python classes
2. Create context structures for state management
3. Define function signatures following [requirements.md](requirements.md) patterns
4. Document ownership and error handling semantics

### Step 3: Implement C Code

For each module:

1. Implement header files with public API declarations
2. Implement source files with full functionality
3. Follow memory management rules (prefer stack allocation)
4. Implement proper error handling and cleanup

### Step 4: Rewrite Tests

For each test file:

1. Convert Python test classes to GoogleTest fixtures
2. Translate `pytest.mark.parametrize` to `::testing::Values(...)`
3. Maintain exact test order from Python version
4. Verify all assertions are equivalent
5. Add C-specific tests at the end (if needed)

### Step 5: Validation

1. Ensure all tests pass
2. Verify thread safety using thread sanitizers
3. Check for memory leaks using valgrind or similar tools
4. Validate against Python implementation behavior

---

## Key Considerations

### Python-to-C Translation Guidelines

- **Classes → Opaque Structs**: Python classes become opaque C structs with create/destroy functions
- **Methods → Functions**: Instance methods become functions taking the instance as first parameter
- **Properties → Accessor Functions**: Python properties become `get_`/`set_` function pairs
- **Exceptions → Error Returns**: Python exceptions become error return values with callbacks
- **Lists/Dicts → Arrays/Custom Structures**: Python collections become C arrays or custom data structures
- **String Handling**: Python strings become `char*` with proper memory management

### Thread Safety Implementation

- Create one context per thread
- Never share contexts across threads
- Store all mutable state in context objects
- Use atomic operations if shared state is unavoidable
- Document thread safety guarantees in headers

### Performance Optimization

- Minimize heap allocations
- Use stack buffers where size is bounded
- Avoid unnecessary string copies
- Implement efficient data structures
- Profile critical paths

---

## Development Environment

> **Note:** If you need to reference the Grammatica Python package during development, use the **existing conda environment** named `grammatica`.

To activate the environment:

```bash
conda activate grammatica
```

---

## CMake Configuration

The C implementation should integrate with CMake, providing:

- Library target (`grammatica`)
- Test executable target (`grammatica_tests`)
- Installation rules
- Package configuration files

Refer to `c/CMakeLists.txt` for the build configuration.

---

## Summary

This is a **complete rewrite project** requiring:

✅ **Full implementation** of all Python modules in C  
✅ **Complete test suite** rewrite in GoogleTest  
✅ **Thread-safe, performant** C library  
✅ **Strict adherence** to [requirements.md](requirements.md) patterns  
✅ **100% functional equivalence** with Python version  
✅ **Comprehensive error handling** and memory management  
✅ **Full test coverage** preservation  

The resulting C library should be a production-ready, high-performance, thread-safe implementation of Grammatica that maintains complete functional parity with the well-tested Python original while following professional C API design standards.
