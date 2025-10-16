# Package Rewrite Instructions

## Task Guidelines

Rewrite the Grammatica Python package (located in [src/grammatica](src/grammatica/)) and its tests (located in [tests/](tests/)) in C (new directory should be located at [c/](c/)).
The C version should be more performant and **must be thread-safe**.

## Requirements

* Use **cmake 3.18** and **C17**.
* Use **GoogleTest 1.17.0** for unit tests.
* The C implementation must be **thread-safe**.
* Prefer parameterized (or "table") tests instead of separating a single test that uses `pytest.mark.parameterize()` (`::testing::Values(...)` in GoogleTest).
* Do not exclude tests that seem trivial or redundant.
* The order of the tests between the C and Python versions must be the same.
* Additional tests that satisfy coverage for the C implementation are allowed, but they must be added at the end of the test file.
* Prefer to allocate memory on the **stack instead of the heap**.
* Try to avoid writing C++ code unless absolutely necessary.

> **Note:** If you need to use the Grammatica Python package then use the **existing conda environment** named `grammatica`.

## Coding Style

* Write idiomatic C code that adheres to common conventions and best practices found in [requirements.md](requirements.md).
  * Reentrant code is preferred.
  * Return type on the same line.

## Summary of Task

In summary, **rewrite the Grammatica Python package and its tests in C**, ensuring that the C implementation is functionally similar to the original while being performant and thread-safe. The new C code should be organized in a new directory and follow the specified coding style and guidelines.
