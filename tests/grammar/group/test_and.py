import pytest

from grammatica.grammar import CharRange, String
from grammatica.grammar.group import (
    And,
    Or,
    merge_adjacent_default_and_grammars,
)

try:
    from ...helpers import fmt_result
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.dirname(path.abspath(__file__)))))
    from tests.helpers import fmt_result


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty And",
            "grammar": And([]),
            "expected": None,
        },
        {
            "description": "Empty And with (0, 1) quantifier",
            "grammar": And([], quantifier=(0, 1)),
            "expected": None,
        },
        {
            "description": "And with subexpressions that all simplify to None returns None",
            "grammar": And([String("")]),
            "expected": None,
        },
        {
            "description": "Adjacent default And grammars are merged after initial simplification",
            "grammar": And(
                [
                    And([String("a"), Or([String("x"), String("y")])]),
                    And([String("b"), CharRange([["0", "9"]])]),
                ],
            ),
            "expected": And(
                [
                    String("a"),
                    Or([String("x"), String("y")]),
                    String("b"),
                    CharRange([["0", "9"]]),
                ],
            ),
        },
        {
            "description": "And with single subexpression",
            "grammar": And([String("a")]),
            "expected": String("a"),
        },
        {
            "description": "And with single subexpression and (0, 1) quantifier",
            "grammar": And([String("a")], quantifier=(0, 1)),
            "expected": And([String("a")], quantifier=(0, 1)),
        },
        {
            "description": "And with consecutive String subexpressions",
            "grammar": And([String("a"), String("b")]),
            "expected": String("ab"),
        },
        {
            "description": "And with consecutive String subexpressions and (0, 1) quantifier",
            "grammar": And([String("a"), String("b")], quantifier=(0, 1)),
            "expected": And([String("ab")], quantifier=(0, 1)),
        },
        {
            "description": "Nested And with consecutive String subexpressions",
            "grammar": And(
                [
                    And([String("a"), String("b")]),
                    And([String("c"), String("d")]),
                ],
            ),
            "expected": String("abcd"),
        },
        {
            "description": "Nested And with (0, 1) quantifiers",
            "grammar": And(
                [
                    And([String("a")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": And([String("a")], quantifier=(0, 1)),
        },
        {
            "description": "Nested And with consecutive String subexpressions and (0, 1) quantifiers",
            "grammar": And(
                [
                    And([String("a"), String("b")], quantifier=(0, 1)),
                    And([String("c"), String("d")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": And(
                [
                    And([String("ab")], quantifier=(0, 1)),
                    And([String("cd")], quantifier=(0, 1)),
                ],
                quantifier=(1, 1),
            ),
        },
        {
            "description": "Nested And with consecutive String subexpressions and (0, 5) quantifiers",
            "grammar": And(
                [
                    And([String("a"), String("b")], quantifier=(0, 5)),
                    And([String("c"), String("d")], quantifier=(0, 5)),
                ],
                quantifier=(0, 1),
            ),
            "expected": And(
                [
                    And([String("ab")], quantifier=(0, 5)),
                    And([String("cd")], quantifier=(0, 5)),
                ],
                quantifier=(1, 1),
            ),
        },
        {
            "description": "Nested And with consecutive String subexpressions and (0, 5) quantifiers",
            "grammar": And(
                [
                    And([String("a"), String("b")], quantifier=(0, 5)),
                    And([String("c"), String("d")], quantifier=(0, 5)),
                ],
                quantifier=(0, 5),
            ),
            "expected": And(
                [
                    And([String("ab")], quantifier=(0, 5)),
                    And([String("cd")], quantifier=(0, 5)),
                ],
                quantifier=(1, 5),
            ),
        },
        {
            "description": "And with simple repeating subexpression",
            "grammar": And(
                [
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                ],
            ),
            "expected": And([Or([String("a"), String("b")])], quantifier=(3, 3)),
        },
        {
            "description": "And with simple repeating subexpression and (0, 1) quantifier",
            "grammar": And(
                [
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                ],
                quantifier=(0, 1),
            ),
            "expected": And(
                [
                    And(
                        [Or([String("a"), String("b")])],
                        quantifier=(3, 3),
                    ),
                ],
                quantifier=(0, 1),
            ),
        },
        {
            "description": "And with simple repeating subexpression and (0, 1) quantifiers",
            "grammar": And(
                [
                    Or([String("a"), String("b")], quantifier=(0, 1)),
                    Or([String("a"), String("b")], quantifier=(0, 1)),
                    Or([String("a"), String("b")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Or([String("a"), String("b")], quantifier=(0, 3)),
        },
        {
            "description": "And with complex repeating subexpression",
            "grammar": And(
                [
                    And([String("a"), String("b")]),
                    And([String("a"), String("b")]),
                    And([String("a"), String("b")]),
                ],
            ),
            "expected": String("ababab"),
        },
        {
            "description": "And with complex repeating subexpression and (0, 1) quantifiers",
            "grammar": And(
                [
                    And([String("a"), String("b")], quantifier=(0, 1)),
                    And([String("a"), String("b")], quantifier=(0, 1)),
                    And([String("a"), String("b")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": And([String("ab")], quantifier=(0, 3)),
        },
        {
            "description": "And with complex repeating subexpression and 0-n quantifiers",
            "grammar": And(
                [
                    And([String("a"), String("b")], quantifier=(0, 3)),
                    And([String("a"), String("b")], quantifier=(0, 4)),
                    And([String("a"), String("b")], quantifier=(0, 5)),
                ],
                quantifier=(0, 2),
            ),
            "expected": And([String("ab")], quantifier=(0, 2 * (3 + 4 + 5))),
        },
        {
            "description": "And with complex repeating subexpressions",
            "grammar": And(
                [
                    # Group 1
                    #   Group 1, Subgroup 1
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    #   Group 1, Subgroup 2
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    And([String("baz")], quantifier=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    #   Group 2, Subgroup 2
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    And([String("baz")], quantifier=(0, 1)),
                ],
            ),
            "expected": And(
                [
                    And(
                        [
                            And([String("foo")], quantifier=(0, 1)),
                            And([String("bar")], quantifier=(0, 1)),
                        ],
                        quantifier=(2, 2),
                    ),
                    And([String("baz")], quantifier=(0, 1)),
                ],
                quantifier=(2, 2),
            ),
        },
        {
            "description": "And with complex repeating subexpressions and surrounding subexpressions",
            "grammar": And(
                [
                    # Leading subexpression
                    And([String("qux")], quantifier=(0, 1)),
                    # Group 1
                    #   Group 1, Subgroup 1
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    #   Group 1, Subgroup 2
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    And([String("baz")], quantifier=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    #   Group 2, Subgroup 2
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    And([String("baz")], quantifier=(0, 1)),
                    # Trailing subexpression
                    And([String("quux")], quantifier=(0, 1)),
                ],
            ),
            "expected": And(
                [
                    And([String("qux")], quantifier=(0, 1)),
                    And(
                        [
                            And(
                                [
                                    And([String("foo")], quantifier=(0, 1)),
                                    And([String("bar")], quantifier=(0, 1)),
                                ],
                                quantifier=(2, 2),
                            ),
                            And([String("baz")], quantifier=(0, 1)),
                        ],
                        quantifier=(2, 2),
                    ),
                    And([String("quux")], quantifier=(0, 1)),
                ],
            ),
        },
        {
            "description": "And with complex repeating subexpressions and (0, 1) quantifier",
            "grammar": And(
                [
                    # Group 1
                    #   Group 1, Subgroup 1
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    #   Group 1, Subgroup 2
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    And([String("baz")], quantifier=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    #   Group 2, Subgroup 2
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    And([String("baz")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": And(
                [
                    And(
                        [
                            And(
                                [
                                    And([String("foo")], quantifier=(0, 1)),
                                    And([String("bar")], quantifier=(0, 1)),
                                ],
                                quantifier=(2, 2),
                            ),
                            And([String("baz")], quantifier=(0, 1)),
                        ],
                        quantifier=(2, 2),
                    ),
                ],
                quantifier=(0, 1),
            ),
        },
        {
            "description": "And with complex repeating subexpressions, (0, 1) quantifier, and surrounding subexpressions",
            "grammar": And(
                [
                    # Leading subexpression
                    And([String("qux")], quantifier=(0, 1)),
                    # Group 1
                    #   Group 1, Subgroup 1
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    #   Group 1, Subgroup 2
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    And([String("baz")], quantifier=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    #   Group 2, Subgroup 2
                    And([String("foo")], quantifier=(0, 1)),
                    And([String("bar")], quantifier=(0, 1)),
                    And([String("baz")], quantifier=(0, 1)),
                    # Trailing subexpression
                    And([String("quux")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": And(
                [
                    And([String("qux")], quantifier=(0, 1)),
                    And(
                        [
                            And(
                                [
                                    And([String("foo")], quantifier=(0, 1)),
                                    And([String("bar")], quantifier=(0, 1)),
                                ],
                                quantifier=(2, 2),
                            ),
                            And([String("baz")], quantifier=(0, 1)),
                        ],
                        quantifier=(2, 2),
                    ),
                    And([String("quux")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
        },
        # {
        #     "description": "And with complex repeating subexpressions, (0, 1) quantifier, and surrounding subexpressions choose best weight",
        #     "grammar": And(...),
        #     "expected": And(...),
        # },
    ],
)
def test_and_simplify(test_case):
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


def test_complex_simplify():
    grammar = And(
        [
            Or(
                [
                    Or(
                        [
                            And([String("a"), String("b")]),
                            And([String("c"), String("d")]),
                        ],
                        quantifier=(0, 1),
                    ),
                    Or(
                        [
                            And([String("e"), String("f")]),
                            And([String("g"), String("h")]),
                        ],
                        quantifier=(0, 1),
                    ),
                ],
                quantifier=(0, 1),
            ),
        ],
        quantifier=(0, 1),
    )
    expected = And(
        [
            Or([String("ab"), String("cd")], quantifier=(0, 1)),
            Or([String("ef"), String("gh")], quantifier=(0, 1)),
        ],
    )
    actual = grammar.simplify()
    assert actual == expected, "\n".join(
        (
            f"Grammar: {fmt_result(grammar)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_complex_simplify2():
    grammar = And(
        [
            And(
                [
                    And(
                        [
                            And([String("a"), String("b")]),
                            And([String("c"), String("d")]),
                        ],
                        quantifier=(0, 1),
                    ),
                    And(
                        [
                            And([String("e"), String("f")]),
                            And([String("g"), String("h")]),
                        ],
                        quantifier=(0, 1),
                    ),
                ],
                quantifier=(0, 1),
            ),
        ],
        quantifier=(0, 1),
    )
    expected = And(
        [
            And([String("abcd")], quantifier=(0, 1)),
            And([String("efgh")], quantifier=(0, 1)),
        ],
    )
    actual = grammar.simplify()
    assert actual == expected, "\n".join(
        (
            f"Grammar: {fmt_result(grammar)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty And",
            "grammar": And([]),
            "expected": None,
        },
        {
            "description": "Empty And with (0, 1) quantifier",
            "grammar": And([], quantifier=(0, 1)),
            "expected": None,
        },
        {
            "description": "And with single subexpression",
            "grammar": And([String("a")]),
            "expected": '"a"',
        },
        {
            "description": "And with single subexpression and (0, 1) quantifier",
            "grammar": And([String("a")], quantifier=(0, 1)),
            "expected": '"a"?',
        },
        {
            "description": "And with multiple subexpressions",
            "grammar": And([String("a"), String("b")]),
            "expected": '"a" "b"',
        },
        {
            "description": "And with multiple subexpressions and (0, 1) quantifier",
            "grammar": And([String("a"), String("b")], quantifier=(0, 1)),
            "expected": '("a" "b")?',
        },
    ],
)
def test_and_render(test_case):
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
            "description": "Empty And",
            "grammar": And([]),
            "expected": False,
        },
        {
            "description": "And with single subexpression and default quantifier",
            "grammar": And([String("a")]),
            "expected": False,
        },
        {
            "description": "And with single subexpression and non-default quantifier",
            "grammar": And([String("a")], quantifier=(0, 1)),
            "expected": False,
        },
        {
            "description": "And with multiple subexpressions and default quantifier",
            "grammar": And([String("a"), String("b")]),
            "expected": False,
        },
        {
            "description": "And with multiple subexpressions and non-default quantifier",
            "grammar": And([String("a"), String("b")], quantifier=(0, 1)),
            "expected": True,
        },
        {
            "description": "Nested And with single subexpression and default quantifier",
            "grammar": And([And([String("a")])]),
            "expected": False,
        },
        {
            "description": "Nested And with single subexpression and non-default quantifier",
            "grammar": And([And([String("a")])], quantifier=(0, 1)),
            "expected": False,
        },
    ],
)
def test_and_needs_wrapped(test_case):
    actual = test_case["grammar"].needs_wrapped()
    assert actual is test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Grammar: {fmt_result(test_case['grammar'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_and_attrs_dict():
    string_a = String("a")
    string_b = String("b")
    grammar = And([string_a, string_b], quantifier=(1, 2))
    expected = {
        "subexprs": [string_a, string_b],
        "quantifier": (1, 2),
    }
    actual = grammar.attrs_dict()
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
            "description": "Single And",
            "subexprs": [And([String("a")])],
            "expected": [And([String("a")])],
        },
        {
            "description": "Multiple adjacent And",
            "subexprs": [
                And([String("a")]),
                And([String("b")]),
                And([String("c")]),
            ],
            "expected": [And([String("a"), String("b"), String("c")])],
        },
        {
            "description": "Mixed adjacent And and other grammars",
            "subexprs": [
                And([String("a")]),
                And([String("b")]),
                Or([String("c"), String("d")]),
                And([String("e")]),
                And([String("f")]),
            ],
            "expected": [
                And([String("a"), String("b")]),
                Or([String("c"), String("d")]),
                And([String("e"), String("f")]),
            ],
        },
        {
            "description": "No adjacent And",
            "subexprs": [
                String("a"),
                And([String("b")]),
                Or([String("c")]),
                String("d"),
            ],
            "expected": [
                String("a"),
                And([String("b")]),
                Or([String("c")]),
                String("d"),
            ],
        },
    ],
)
def test_merge_adjacent_default_and_grammars(test_case):
    subexprs = [expr.copy() for expr in test_case["subexprs"]]
    n = len(subexprs)
    new_n = merge_adjacent_default_and_grammars(subexprs, n)
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
