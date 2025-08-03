import pytest

from grammatica.constants import (
    ALWAYS_SAFE_CHARS,
    CHAR_ESCAPE_MAP,
    STRING_LITERAL_ESCAPE_CHARS,
)
from grammatica.grammar.string import String

from .helpers import grammar_to_string


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty",
            "grammar": String(""),
            "expected": None,
        },
        {
            "description": "Single character",
            "grammar": String("a"),
            "expected": '"a"',
        },
        {
            "description": "Multiple characters",
            "grammar": String("abc"),
            "expected": '"abc"',
        },
        {
            "description": "Always safe characters",
            "grammar": String("".join(ALWAYS_SAFE_CHARS)),
            "expected": '"{}"'.format("".join(ALWAYS_SAFE_CHARS)),
        },
        {
            "description": "Escape general tokens",
            "grammar": String("".join(CHAR_ESCAPE_MAP)),
            "expected": '"{}"'.format("".join(CHAR_ESCAPE_MAP.values())),
        },
        {
            "description": "Escape string literal characters",
            "grammar": String("".join(STRING_LITERAL_ESCAPE_CHARS)),
            "expected": '"{}"'.format(
                "".join(f"\\{c}" for c in STRING_LITERAL_ESCAPE_CHARS)
            ),
        },
        {
            "description": "Escape non-ASCII characters",
            "grammar": String("".join(chr(i) for i in range(127462, 127462 + 26))),
            "expected": '"{}"'.format(
                "".join(f"\\x{i:02X}" for i in range(127462, 127462 + 26))
            ),
        },
    ],
)
def test_string_render(test_case):
    actual = test_case["grammar"].render()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Grammar: {grammar_to_string(test_case['grammar'])!s}",
            f"Expected: {grammar_to_string(test_case['expected'])!s}",
            f"Actual: {grammar_to_string(actual)!s}",
        )
    )


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty",
            "grammar": String(""),
            "expected": None,
        },
        {
            "description": "Single character",
            "grammar": String("a"),
            "expected": String("a"),
        },
        {
            "description": "Multiple characters",
            "grammar": String("abc"),
            "expected": String("abc"),
        },
    ],
)
def test_string_simplify(test_case):
    actual = test_case["grammar"].simplify()
    assert (actual == test_case["expected"]) and (
        (test_case["expected"] is not actual)
        or (test_case["expected"] is actual is None)
    ), "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Grammar: {grammar_to_string(test_case['grammar'])!s}",
            f"Expected: {grammar_to_string(test_case['expected'])!s}",
            f"Actual: {grammar_to_string(actual)!s}",
        )
    )
