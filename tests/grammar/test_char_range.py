import pytest

from grammatica.grammar.char_range import CharRange
from grammatica.grammar.string import String

from .helpers import grammar_to_string


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "CharRange with single character",
            "grammar": CharRange([("a", "a")]),
            "expected": String("a"),
        },
        {
            "description": "CharRange with single character range",
            "grammar": CharRange([("a", "z")]),
            "expected": CharRange([("a", "z")]),
        },
    ],
)
def test_grammar_simplify(test_case):
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
