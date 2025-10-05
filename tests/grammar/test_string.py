import pytest

from grammatica.constants import (
    ALWAYS_SAFE_CHARS,
    CHAR_ESCAPE_MAP,
    STRING_LITERAL_ESCAPE_CHARS,
)
from grammatica.grammar.grammar import Grammar
from grammatica.grammar.string import String, merge_adjacent_strings

try:
    from .helpers import fmt_result
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))
    from helpers import fmt_result


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
            "expected": '"{}"'.format("".join(f"\\{c}" for c in STRING_LITERAL_ESCAPE_CHARS)),
        },
        {
            "description": "Escape non-ASCII characters",
            "grammar": String("".join(chr(i) for i in range(127462, 127462 + 26))),
            "expected": '"{}"'.format("".join(f"\\x{i:02X}" for i in range(127462, 127462 + 26))),
        },
    ],
)
def test_string_render(test_case):
    actual = test_case["grammar"].render()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Grammar: {fmt_result(test_case['grammar'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
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
        (test_case["expected"] is not actual) or (test_case["expected"] is actual is None)
    ), "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Grammar: {fmt_result(test_case['grammar'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_string_attrs_dict():
    string = String("test")
    assert string.attrs_dict() == {"value": "test"}


def test_string_as_string():
    string = String("test")
    assert string.as_string(indent=None) == string.as_string(indent=2) == "String(value='test')"


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty list",
            "subexprs": [],
            "expected": [],
        },
        {
            "description": "Single String",
            "subexprs": [String("a")],
            "expected": [String("a")],
        },
        {
            "description": "Multiple adjacent String",
            "subexprs": [String("a"), String("b"), String("c")],
            "expected": [String("abc")],
        },
        {
            "description": "Mixed adjacent String and other grammars",
            "subexprs": [String("a"), String("b"), Grammar([String("c")]), String("d")],
            "expected": [String("ab"), Grammar([String("c")]), String("d")],
        },
        {
            "description": "No adjacent String",
            "subexprs": [Grammar([String("a")]), Grammar([String("b")])],
            "expected": [Grammar([String("a")]), Grammar([String("b")])],
        },
    ],
)
def test_merge_adjacent_strings(test_case):
    subexprs = [expr.copy() for expr in test_case["subexprs"]]
    n = len(subexprs)
    new_n = merge_adjacent_strings(subexprs, n)
    assert new_n == len(test_case["expected"]), "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Original: {n!s}",
            f"Expected: {len(test_case['expected'])!s}",
            f"Actual: {new_n!s}",
        )
    )
    assert subexprs == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Original: {fmt_result(test_case['subexprs'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(subexprs)!s}",
        )
    )
