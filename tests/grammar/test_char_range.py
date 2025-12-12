import pytest

from grammatica.constants import (
    ALWAYS_SAFE_CHARS,
    CHAR_ESCAPE_MAP,
    RANGE_ESCAPE_CHARS,
)
from grammatica.grammar.char_range import CharRange
from grammatica.grammar.string import String

try:
    from .helpers import fmt_result
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))
    from helpers import fmt_result


def test_char_range_render_empty():
    grammar = CharRange([("a", "z")])
    grammar.char_ranges = []
    assert grammar.render() is None


def test_char_range_from_chars():
    assert CharRange.from_chars("abcxyz") == CharRange([("a", "c"), ("x", "z")])


def test_char_range_from_chars_negated():
    assert CharRange.from_chars("xyz", negate=True) == CharRange([("x", "z")], negate=True)


def test_char_range_from_ords():
    assert CharRange.from_ords([97, 98, 99, 120, 121, 122]) == CharRange([("a", "c"), ("x", "z")])


def test_char_range_from_ords_negated():
    assert CharRange.from_ords([97, 98, 99, 120, 121, 122], negate=True) == CharRange([("a", "c"), ("x", "z")], negate=True)


def test_char_range_validation_empty():
    with pytest.raises(ValueError, match="char_ranges must not be empty"):
        CharRange([])


def test_char_range_validation_start_length():
    with pytest.raises(ValueError, match="start must be a single character"):
        CharRange([("ab", "z")])


def test_char_range_validation_end_length():
    with pytest.raises(ValueError, match="end must be a single character"):
        CharRange([("a", "zz")])


def test_char_range_validation_end_before_start():
    with pytest.raises(ValueError, match="end must be greater than or equal to start"):
        CharRange([("z", "a")])


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Single character simplifies to String",
            "grammar": CharRange([("a", "a")]),
            "expected": String("a"),
        },
        {
            "description": "CharRange stays as CharRange",
            "grammar": CharRange([("a", "z")]),
            "expected": CharRange([("a", "z")]),
        },
        {
            "description": "Multiple ranges stay as CharRange",
            "grammar": CharRange([("a", "c"), ("x", "z")]),
            "expected": CharRange([("a", "c"), ("x", "z")]),
        },
        {
            "description": "Overlapping ranges merge",
            "grammar": CharRange([("a", "c"), ("b", "e")]),
            "expected": CharRange([("a", "e")]),
        },
    ],
)
def test_char_range_simplify(test_case):
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


def test_char_range_simplify_empty():
    grammar = CharRange([("a", "z")])
    grammar.char_ranges = []
    assert grammar.simplify() is None


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Simple range",
            "grammar": CharRange([("a", "z")]),
            "expected": "[a-z]",
        },
        {
            "description": "Single character",
            "grammar": CharRange([("a", "a")]),
            "expected": "[a]",
        },
        {
            "description": "Multiple ranges",
            "grammar": CharRange([("a", "z"), ("0", "9")]),
            "expected": "[0-9a-z]",
        },
        {
            "description": "Negated range",
            "grammar": CharRange([("a", "z")], negate=True),
            "expected": "[^a-z]",
        },
        {
            "description": "Adjacent characters",
            "grammar": CharRange([("a", "b")]),
            "expected": "[ab]",
        },
    ]
    + [
        {
            "description": f"Character is always safe in ranges: {char!r}",
            "grammar": CharRange([(char, char)]),
            "expected": f"[{char}]",
        }
        for char in ALWAYS_SAFE_CHARS - frozenset(CHAR_ESCAPE_MAP) - RANGE_ESCAPE_CHARS
    ]
    + [
        {
            "description": f"Character is always escaped in ranges: {char!r}",
            "grammar": CharRange([(char, char)]),
            "expected": f"[{CHAR_ESCAPE_MAP[char]}]",
        }
        for char in frozenset(CHAR_ESCAPE_MAP) - RANGE_ESCAPE_CHARS
    ]
    + [
        {
            "description": f"Character is always simple-escaped in a ranges: {char!r}",
            "grammar": CharRange([(char, char)]),
            "expected": f"[\\{char}]",
        }
        for char in RANGE_ESCAPE_CHARS
    ]
    + [
        {
            "description": f"Other character is fully-escaped in ranges: {char!r}",
            "grammar": CharRange([(char, char)]),
            "expected": f"[\\x{ord(char):02X}]",
        }
        for char in map(chr, frozenset({0x00, 0x01, 0x02, 0x03, 0x04, 0x7F, 0x80, 0x81, 0xFF}))
    ],
)
def test_char_range_render(test_case):
    actual = test_case["grammar"].render()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Grammar: {fmt_result(test_case['grammar'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_char_range_as_string():
    grammar = CharRange([("a", "z")])
    assert grammar.as_string(indent=None) == grammar.as_string(indent=2) == "CharRange(char_ranges=[('a', 'z')], negate=False)"


def test_char_range_attrs_dict():
    grammar = CharRange([("a", "z")])
    assert grammar.attrs_dict() == {
        "char_ranges": [("a", "z")],
        "negate": False,
    }
