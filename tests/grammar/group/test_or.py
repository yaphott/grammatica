import pytest

from grammatica.grammar import String
from grammatica.grammar.group import (
    And,
    Or,
    merge_adjacent_default_or_grammars,
)

try:
    from ..helpers import fmt_result
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.dirname(path.abspath(__file__)))))
    from helpers import fmt_result


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty Or",
            "grammar": Or([]),
            "expected": None,
        },
        {
            "description": "Empty Or with (0, 1) quantifier",
            "grammar": Or([], quantifier=(0, 1)),
            "expected": None,
        },
        {
            "description": "Or with subexpressions that all simplify to None returns None",
            "grammar": Or([String("")]),
            "expected": None,
        },
        {
            "description": "Or with single subexpression",
            "grammar": Or([String("a")]),
            "expected": String("a"),
        },
        {
            "description": "Or with single subexpression and (0, 1) quantifier",
            "grammar": Or([String("a")], quantifier=(0, 1)),
            "expected": And([String("a")], quantifier=(0, 1)),
        },
        {
            "description": "Or with consecutive String subexpressions",
            "grammar": Or([String("a"), String("b")]),
            "expected": Or([String("a"), String("b")]),
        },
        {
            "description": "Or with consecutive String subexpressions and (0, 1) quantifier",
            "grammar": Or([String("a"), String("b")], quantifier=(0, 1)),
            "expected": Or([String("a"), String("b")], quantifier=(0, 1)),
        },
        {
            "description": "Nested Or with consecutive String subexpressions",
            "grammar": Or(
                [
                    Or([String("a"), String("b")]),
                    Or([String("c"), String("d")]),
                ],
            ),
            "expected": Or([String("a"), String("b"), String("c"), String("d")]),
        },
        {
            "description": "Nested Or with (0, 1) quantifiers",
            "grammar": Or(
                [
                    Or([String("a")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": And([String("a")], quantifier=(0, 1)),
        },
        {
            "description": "Nested Or with consecutive String subexpressions and (0, 1) quantifiers",
            "grammar": Or(
                [
                    Or([String("a"), String("b")], quantifier=(0, 1)),
                    Or([String("c"), String("d")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Or(
                [
                    Or([String("a"), String("b")], quantifier=(0, 1)),
                    Or([String("c"), String("d")], quantifier=(0, 1)),
                ],
                quantifier=(1, 1),
            ),
        },
        {
            "description": "Nested Or with consecutive String subexpressions and (0, 5) quantifiers",
            "grammar": Or(
                [
                    Or([String("a"), String("b")], quantifier=(0, 5)),
                    Or([String("c"), String("d")], quantifier=(0, 5)),
                ],
                quantifier=(0, 5),
            ),
            "expected": Or(
                [
                    Or([String("a"), String("b")], quantifier=(0, 5)),
                    Or([String("c"), String("d")], quantifier=(0, 5)),
                ],
                quantifier=(1, 5),
            ),
        },
        {
            "description": "Or with simple repeating subexpression",
            "grammar": Or([String("a"), String("a"), String("a")]),
            "expected": String("a"),
        },
        {
            "description": "Or with simple repeating subexpression and (0, 1) quantifier",
            "grammar": Or([String("a"), String("a"), String("a")], quantifier=(0, 1)),
            "expected": And([String("a")], quantifier=(0, 1)),
        },
        {
            "description": "Or with complex repeating subexpression",
            "grammar": Or(
                [
                    And([String("a"), String("b")]),
                    And([String("a"), String("b")]),
                    And([String("a"), String("b")]),
                ],
            ),
            "expected": String("ab"),
        },
        {
            "description": "Or with complex repeating subexpression and (0, 1) quantifiers",
            "grammar": Or(
                [
                    And([String("a"), String("b")], quantifier=(0, 1)),
                    And([String("a"), String("b")], quantifier=(0, 1)),
                    And([String("a"), String("b")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": And([String("ab")], quantifier=(0, 1)),
        },
    ],
)
def test_or_simplify(test_case):
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


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty Or",
            "grammar": Or([]),
            "expected": None,
        },
        {
            "description": "Empty Or with (0, 1) quantifier",
            "grammar": Or([], quantifier=(0, 1)),
            "expected": None,
        },
        {
            "description": "Or with multiple subexpressions",
            "grammar": Or([String("a"), String("b")]),
            "expected": '("a" | "b")',
        },
        {
            "description": "Or with multiple subexpressions and (0, 1) quantifier",
            "grammar": Or([String("a"), String("b")], quantifier=(0, 1)),
            "expected": '("a" | "b")?',
        },
        {
            "description": "Or with multiple subexpressions and (0, None) quantifier",
            "grammar": Or([String("a"), String("b")], quantifier=(0, None)),
            "expected": '("a" | "b")*',
        },
        {
            "description": "Or with multiple subexpressions and (1, None) quantifier",
            "grammar": Or([String("a"), String("b")], quantifier=(1, None)),
            "expected": '("a" | "b")+',
        },
        {
            "description": "Or with multiple subexpressions and (1, 3) quantifier",
            "grammar": Or([String("a"), String("b")], quantifier=(1, 3)),
            "expected": '("a" | "b"){1,3}',
        },
        {
            "description": "Or with single subexpression",
            "grammar": Or([String("a")]),
            "expected": '"a"',
        },
        {
            "description": "Or with single subexpression and (0, 1) quantifier",
            "grammar": Or([String("a")], quantifier=(0, 1)),
            "expected": '"a"?',
        },
    ],
)
def test_or_render(test_case):
    actual = test_case["grammar"].render()
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


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty Or",
            "grammar": Or([]),
            "expected": False,
        },
        {
            "description": "Or with single subexpression and default quantifier",
            "grammar": Or([String("a")]),
            "expected": False,
        },
        {
            "description": "Or with single subexpression and non-default quantifier",
            "grammar": Or([String("a")], quantifier=(0, 1)),
            "expected": False,
        },
        {
            "description": "Or with multiple subexpressions and default quantifier",
            "grammar": Or([String("a"), String("b")]),
            "expected": True,
        },
        {
            "description": "Or with multiple subexpressions and non-default quantifier",
            "grammar": Or([String("a"), String("b")], quantifier=(0, 1)),
            "expected": True,
        },
        {
            "description": "Nested Or with single subexpression and default quantifier",
            "grammar": Or([Or([String("a")])]),
            "expected": False,
        },
        {
            "description": "Nested Or with single subexpression and non-default quantifier",
            "grammar": Or([Or([String("a")])], quantifier=(0, 1)),
            "expected": False,
        },
    ],
)
def test_or_needs_wrapped(test_case):
    actual = test_case["grammar"].needs_wrapped()
    assert actual is test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Grammar: {fmt_result(test_case['grammar'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_or_attrs_dict():
    string_a = String("a")
    string_b = String("b")
    or_expr = Or([string_a, string_b], quantifier=(1, 2))
    expected = {
        "subexprs": [string_a, string_b],
        "quantifier": (1, 2),
    }
    actual = or_expr.attrs_dict()
    assert actual == expected
    for i in range(len(actual["subexprs"])):
        assert actual["subexprs"][i] is expected["subexprs"][i]


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty list",
            "subexprs": [],
            "expected": [],
        },
        {
            "description": "Single Or",
            "subexprs": [Or([String("a")])],
            "expected": [Or([String("a")])],
        },
        {
            "description": "Multiple adjacent Or",
            "subexprs": [Or([String("a")]), Or([String("b")]), Or([String("c")])],
            "expected": [Or([String("a"), String("b"), String("c")])],
        },
        {
            "description": "Mixed adjacent Or and other grammars",
            "subexprs": [
                Or([String("a")]),
                Or([String("b")]),
                And([String("c"), String("d")]),
                Or([String("e")]),
                Or([String("f")]),
            ],
            "expected": [
                Or([String("a"), String("b")]),
                And([String("c"), String("d")]),
                Or([String("e"), String("f")]),
            ],
        },
        {
            "description": "No adjacent Or",
            "subexprs": [
                String("a"),
                Or([String("b")]),
                And([String("c")]),
                String("d"),
            ],
            "expected": [
                String("a"),
                Or([String("b")]),
                And([String("c")]),
                String("d"),
            ],
        },
    ],
)
def test_merge_adjacent_default_or_grammars(test_case):
    subexprs = [expr.copy() for expr in test_case["subexprs"]]
    n = len(subexprs)
    new_n = merge_adjacent_default_or_grammars(subexprs, n)
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
