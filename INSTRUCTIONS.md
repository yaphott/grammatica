# Package Rewrite Instructions

## Task Guidelines

Rewrite the Grammatica Python package (located in `./src/grammatica/`) and its tests (located in `./tests/`) in C (located in `./c/`).
The C version should be more performant and **must be thread-safe**.

## Requirements

* Use **cmake 3.18** and **C17**.
* Prefer parameterized (or "table") tests instead of separating a single test that uses `pytest.mark.parameterize()` (`::testing::Values(...)` in GoogleTest).
* Do not exclude tests that seem trivial or redundant.
* The order of the tests between the C and Python versions must be the same.
* Additional tests that satisfy coverage for the C implementation are allowed, but they must be added at the end of the test file.
* Prefer to allocate memory on the **stack instead of the heap**.

> **Note:** If you need to use the Python package, or run any tests, use the existing conda environment named `grammatica-clean`.

## Coding Style

* Write idiomatic C code that adheres to common conventions and best practices found in `./c/LIBGEOS_C_API_STYLE_GUIDE.md`.
* There is no limit on line length, but try to keep it under 140 characters.
* Use 4 spaces for indentation.
* Do not use multiple consecutive newlines in functions or methods.
* Use `type* parameter` instead of `type *parameter`.
* Use `if (condition)` instead of `if(condition)`.
* Use `for (initialization; condition; increment)` instead of `for(initialization; condition; increment)`.
* Use `while (condition)` instead of `while(condition)`.
* Use `function_name()` instead of `function_name ()`.

## Summary of Task

In summary, **rewrite the Grammatica Python package and its tests in C**, ensuring that the C implementation is functionally similar to the original while being performant and thread-safe. The new C code should be organized in a new directory and follow the specified coding style and guidelines.
