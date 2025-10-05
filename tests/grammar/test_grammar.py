import pytest

from grammatica.grammar.char_range import CharRange
from grammatica.grammar.grammar import (
    Grammar,
    Or,
    merge_adjacent_default_grammar,
    merge_adjacent_default_or,
)
from grammatica.grammar.string import String

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
            "description": "Empty Grammar",
            "grammar": Grammar([]),
            "expected": None,
        },
        {
            "description": "Empty Grammar with (0, 1) quantifier",
            "grammar": Grammar([], quantifier=(0, 1)),
            "expected": None,
        },
        {
            "description": "Grammar with subexpressions that all simplify to None returns None",
            "grammar": Grammar([String("")]),
            "expected": None,
        },
        {
            "description": "Adjacent default grammars are merged after initial simplification",
            "grammar": Grammar(
                [
                    Grammar([String("a"), Or([String("x"), String("y")])]),
                    Grammar([String("b"), CharRange([["0", "9"]])]),
                ],
            ),
            "expected": Grammar(
                [
                    String("a"),
                    Or([String("x"), String("y")]),
                    String("b"),
                    CharRange([["0", "9"]]),
                ],
            ),
        },
        {
            "description": "Grammar with single subexpression",
            "grammar": Grammar([String("a")]),
            "expected": String("a"),
        },
        {
            "description": "Grammar with single subexpression and (0, 1) quantifier",
            "grammar": Grammar([String("a")], quantifier=(0, 1)),
            "expected": Grammar([String("a")], quantifier=(0, 1)),
        },
        {
            "description": "Grammar with consecutive String subexpressions",
            "grammar": Grammar([String("a"), String("b")]),
            "expected": String("ab"),
        },
        {
            "description": "Grammar with consecutive String subexpressions and (0, 1) quantifier",
            "grammar": Grammar([String("a"), String("b")], quantifier=(0, 1)),
            "expected": Grammar([String("ab")], quantifier=(0, 1)),
        },
        {
            "description": "Nested Grammar with consecutive String subexpressions",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")]),
                    Grammar([String("c"), String("d")]),
                ],
            ),
            "expected": String("abcd"),
        },
        {
            "description": "Nested Grammar with (0, 1) quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Grammar([String("a")], quantifier=(0, 1)),
        },
        {
            "description": "Nested Grammar with consecutive String subexpressions and (0, 1) quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], quantifier=(0, 1)),
                    Grammar([String("c"), String("d")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar([String("ab")], quantifier=(0, 1)),
                    Grammar([String("cd")], quantifier=(0, 1)),
                ],
                quantifier=(1, 1),
            ),
        },
        {
            "description": "Nested Grammar with consecutive String subexpressions and (0, 5) quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], quantifier=(0, 5)),
                    Grammar([String("c"), String("d")], quantifier=(0, 5)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar([String("ab")], quantifier=(0, 5)),
                    Grammar([String("cd")], quantifier=(0, 5)),
                ],
                quantifier=(1, 1),
            ),
        },
        {
            "description": "Nested Grammar with consecutive String subexpressions and (0, 5) quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], quantifier=(0, 5)),
                    Grammar([String("c"), String("d")], quantifier=(0, 5)),
                ],
                quantifier=(0, 5),
            ),
            "expected": Grammar(
                [
                    Grammar([String("ab")], quantifier=(0, 5)),
                    Grammar([String("cd")], quantifier=(0, 5)),
                ],
                quantifier=(1, 5),
            ),
        },
        {
            "description": "Grammar with simple repeating subexpression",
            "grammar": Grammar(
                [
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                ],
            ),
            "expected": Grammar([Or([String("a"), String("b")])], quantifier=(3, 3)),
        },
        {
            "description": "Grammar with simple repeating subexpression and (0, 1) quantifier",
            "grammar": Grammar(
                [
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                ],
                quantifier=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar(
                        [Or([String("a"), String("b")])],
                        quantifier=(3, 3),
                    ),
                ],
                quantifier=(0, 1),
            ),
        },
        {
            "description": "Grammar with simple repeating subexpression and (0, 1) quantifiers",
            "grammar": Grammar(
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
            "description": "Grammar with complex repeating subexpression",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")]),
                    Grammar([String("a"), String("b")]),
                    Grammar([String("a"), String("b")]),
                ],
            ),
            "expected": String("ababab"),
        },
        {
            "description": "Grammar with complex repeating subexpression and (0, 1) quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], quantifier=(0, 1)),
                    Grammar([String("a"), String("b")], quantifier=(0, 1)),
                    Grammar([String("a"), String("b")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Grammar([String("ab")], quantifier=(0, 3)),
        },
        {
            "description": "Grammar with complex repeating subexpression and 0-n quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], quantifier=(0, 3)),
                    Grammar([String("a"), String("b")], quantifier=(0, 4)),
                    Grammar([String("a"), String("b")], quantifier=(0, 5)),
                ],
                quantifier=(0, 2),
            ),
            "expected": Grammar([String("ab")], quantifier=(0, 2 * (3 + 4 + 5))),
        },
        {
            "description": "Grammar with complex repeating subexpressions",
            "grammar": Grammar(
                [
                    # Group 1
                    #   Group 1, Subgroup 1
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    #   Group 1, Subgroup 2
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    Grammar([String("baz")], quantifier=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    #   Group 2, Subgroup 2
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    Grammar([String("baz")], quantifier=(0, 1)),
                ],
            ),
            "expected": Grammar(
                [
                    Grammar(
                        [
                            Grammar([String("foo")], quantifier=(0, 1)),
                            Grammar([String("bar")], quantifier=(0, 1)),
                        ],
                        quantifier=(2, 2),
                    ),
                    Grammar([String("baz")], quantifier=(0, 1)),
                ],
                quantifier=(2, 2),
            ),
        },
        {
            "description": "Grammar with complex repeating subexpressions and surrounding subexpressions",
            "grammar": Grammar(
                [
                    # Leading subexpression
                    Grammar([String("qux")], quantifier=(0, 1)),
                    # Group 1
                    #   Group 1, Subgroup 1
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    #   Group 1, Subgroup 2
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    Grammar([String("baz")], quantifier=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    #   Group 2, Subgroup 2
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    Grammar([String("baz")], quantifier=(0, 1)),
                    # Trailing subexpression
                    Grammar([String("quux")], quantifier=(0, 1)),
                ],
            ),
            "expected": Grammar(
                [
                    Grammar([String("qux")], quantifier=(0, 1)),
                    Grammar(
                        [
                            Grammar(
                                [
                                    Grammar([String("foo")], quantifier=(0, 1)),
                                    Grammar([String("bar")], quantifier=(0, 1)),
                                ],
                                quantifier=(2, 2),
                            ),
                            Grammar([String("baz")], quantifier=(0, 1)),
                        ],
                        quantifier=(2, 2),
                    ),
                    Grammar([String("quux")], quantifier=(0, 1)),
                ],
            ),
        },
        {
            "description": "Grammar with complex repeating subexpressions and (0, 1) quantifier",
            "grammar": Grammar(
                [
                    # Group 1
                    #   Group 1, Subgroup 1
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    #   Group 1, Subgroup 2
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    Grammar([String("baz")], quantifier=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    #   Group 2, Subgroup 2
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    Grammar([String("baz")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar(
                        [
                            Grammar(
                                [
                                    Grammar([String("foo")], quantifier=(0, 1)),
                                    Grammar([String("bar")], quantifier=(0, 1)),
                                ],
                                quantifier=(2, 2),
                            ),
                            Grammar([String("baz")], quantifier=(0, 1)),
                        ],
                        quantifier=(2, 2),
                    ),
                ],
                quantifier=(0, 1),
            ),
        },
        {
            "description": "Grammar with complex repeating subexpressions, (0, 1) quantifier, and surrounding subexpressions",
            "grammar": Grammar(
                [
                    # Leading subexpression
                    Grammar([String("qux")], quantifier=(0, 1)),
                    # Group 1
                    #   Group 1, Subgroup 1
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    #   Group 1, Subgroup 2
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    Grammar([String("baz")], quantifier=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    #   Group 2, Subgroup 2
                    Grammar([String("foo")], quantifier=(0, 1)),
                    Grammar([String("bar")], quantifier=(0, 1)),
                    Grammar([String("baz")], quantifier=(0, 1)),
                    # Trailing subexpression
                    Grammar([String("quux")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar([String("qux")], quantifier=(0, 1)),
                    Grammar(
                        [
                            Grammar(
                                [
                                    Grammar([String("foo")], quantifier=(0, 1)),
                                    Grammar([String("bar")], quantifier=(0, 1)),
                                ],
                                quantifier=(2, 2),
                            ),
                            Grammar([String("baz")], quantifier=(0, 1)),
                        ],
                        quantifier=(2, 2),
                    ),
                    Grammar([String("quux")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
        },
        # {
        #     "description": "Grammar with complex repeating subexpressions, (0, 1) quantifier, and surrounding subexpressions choose best weight",
        #     "grammar": Grammar(...),
        #     "expected": Grammar(...),
        # },
    ],
)
def test_grammar_simplify(test_case):
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
            "expected": Grammar([String("a")], quantifier=(0, 1)),
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
            "expected": Grammar([String("a")], quantifier=(0, 1)),
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
            "expected": Grammar([String("a")], quantifier=(0, 1)),
        },
        {
            "description": "Or with complex repeating subexpression",
            "grammar": Or(
                [
                    Grammar([String("a"), String("b")]),
                    Grammar([String("a"), String("b")]),
                    Grammar([String("a"), String("b")]),
                ],
            ),
            "expected": String("ab"),
        },
        {
            "description": "Or with complex repeating subexpression and (0, 1) quantifiers",
            "grammar": Or(
                [
                    Grammar([String("a"), String("b")], quantifier=(0, 1)),
                    Grammar([String("a"), String("b")], quantifier=(0, 1)),
                    Grammar([String("a"), String("b")], quantifier=(0, 1)),
                ],
                quantifier=(0, 1),
            ),
            "expected": Grammar([String("ab")], quantifier=(0, 1)),
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


def test_complex_simplify():
    grammar = Grammar(
        [
            Or(
                [
                    Or(
                        [
                            Grammar([String("a"), String("b")]),
                            Grammar([String("c"), String("d")]),
                        ],
                        quantifier=(0, 1),
                    ),
                    Or(
                        [
                            Grammar([String("e"), String("f")]),
                            Grammar([String("g"), String("h")]),
                        ],
                        quantifier=(0, 1),
                    ),
                ],
                quantifier=(0, 1),
            ),
        ],
        quantifier=(0, 1),
    )
    expected = Or(
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
    grammar = Grammar(
        [
            Grammar(
                [
                    Grammar(
                        [
                            Grammar([String("a"), String("b")]),
                            Grammar([String("c"), String("d")]),
                        ],
                        quantifier=(0, 1),
                    ),
                    Grammar(
                        [
                            Grammar([String("e"), String("f")]),
                            Grammar([String("g"), String("h")]),
                        ],
                        quantifier=(0, 1),
                    ),
                ],
                quantifier=(0, 1),
            ),
        ],
        quantifier=(0, 1),
    )
    expected = Grammar(
        [
            Grammar([String("abcd")], quantifier=(0, 1)),
            Grammar([String("efgh")], quantifier=(0, 1)),
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
            "description": "Empty Grammar",
            "grammar": Grammar([]),
            "expected": None,
        },
        {
            "description": "Empty Grammar with (0, 1) quantifier",
            "grammar": Grammar([], quantifier=(0, 1)),
            "expected": None,
        },
        {
            "description": "Grammar with single subexpression",
            "grammar": Grammar([String("a")]),
            "expected": '"a"',
        },
        {
            "description": "Grammar with single subexpression and (0, 1) quantifier",
            "grammar": Grammar([String("a")], quantifier=(0, 1)),
            "expected": '"a"?',
        },
        {
            "description": "Grammar with multiple subexpressions",
            "grammar": Grammar([String("a"), String("b")]),
            "expected": '"a" "b"',
        },
        {
            "description": "Grammar with multiple subexpressions and (0, 1) quantifier",
            "grammar": Grammar([String("a"), String("b")], quantifier=(0, 1)),
            "expected": '("a" "b")?',
        },
    ],
)
def test_grammar_render(test_case):
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
            "description": "Empty Grammar",
            "grammar": Grammar([]),
            "expected": False,
        },
        {
            "description": "Grammar with single subexpression and default quantifier",
            "grammar": Grammar([String("a")]),
            "expected": False,
        },
        {
            "description": "Grammar with single subexpression and non-default quantifier",
            "grammar": Grammar([String("a")], quantifier=(0, 1)),
            "expected": False,
        },
        {
            "description": "Grammar with multiple subexpressions and default quantifier",
            "grammar": Grammar([String("a"), String("b")]),
            "expected": False,
        },
        {
            "description": "Grammar with multiple subexpressions and non-default quantifier",
            "grammar": Grammar([String("a"), String("b")], quantifier=(0, 1)),
            "expected": True,
        },
        {
            "description": "Nested Grammar with single subexpression and default quantifier",
            "grammar": Grammar([Grammar([String("a")])]),
            "expected": False,
        },
        {
            "description": "Nested Grammar with single subexpression and non-default quantifier",
            "grammar": Grammar([Grammar([String("a")])], quantifier=(0, 1)),
            "expected": False,
        },
    ],
)
def test_grammar_needs_wrapped(test_case):
    actual = test_case["grammar"].needs_wrapped()
    assert actual is test_case["expected"], "\n".join(
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


def test_grammar_attrs_dict():
    string_a = String("a")
    string_b = String("b")
    grammar = Grammar([string_a, string_b], quantifier=(1, 2))
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
            "description": "Single Grammar",
            "subexprs": [Grammar([String("a")])],
            "expected": [Grammar([String("a")])],
        },
        {
            "description": "Multiple adjacent Grammar",
            "subexprs": [
                Grammar([String("a")]),
                Grammar([String("b")]),
                Grammar([String("c")]),
            ],
            "expected": [Grammar([String("a"), String("b"), String("c")])],
        },
        {
            "description": "Mixed adjacent Grammar and other grammars",
            "subexprs": [
                Grammar([String("a")]),
                Grammar([String("b")]),
                Or([String("c"), String("d")]),
                Grammar([String("e")]),
                Grammar([String("f")]),
            ],
            "expected": [
                Grammar([String("a"), String("b")]),
                Or([String("c"), String("d")]),
                Grammar([String("e"), String("f")]),
            ],
        },
        {
            "description": "No adjacent Grammar",
            "subexprs": [
                String("a"),
                Grammar([String("b")]),
                Or([String("c")]),
                String("d"),
            ],
            "expected": [
                String("a"),
                Grammar([String("b")]),
                Or([String("c")]),
                String("d"),
            ],
        },
    ],
)
def test_merge_adjacent_default_grammar(test_case):
    subexprs = [expr.copy() for expr in test_case["subexprs"]]
    n = len(subexprs)
    new_n = merge_adjacent_default_grammar(subexprs, n)
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
                Grammar([String("c"), String("d")]),
                Or([String("e")]),
                Or([String("f")]),
            ],
            "expected": [
                Or([String("a"), String("b")]),
                Grammar([String("c"), String("d")]),
                Or([String("e"), String("f")]),
            ],
        },
        {
            "description": "No adjacent Or",
            "subexprs": [
                String("a"),
                Or([String("b")]),
                Grammar([String("c")]),
                String("d"),
            ],
            "expected": [
                String("a"),
                Or([String("b")]),
                Grammar([String("c")]),
                String("d"),
            ],
        },
    ],
)
def test_merge_adjacent_default_or(test_case):
    subexprs = [expr.copy() for expr in test_case["subexprs"]]
    n = len(subexprs)
    new_n = merge_adjacent_default_or(subexprs, n)
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
