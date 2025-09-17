import pytest

from grammatica.grammar.grammar import Grammar, Or
from grammatica.grammar.string import String

from .helpers import grammar_to_string


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
            "grammar": Grammar([], length_range=(0, 1)),
            "expected": None,
        },
        {
            "description": "Grammar with single subexpression",
            "grammar": Grammar([String("a")]),
            "expected": String("a"),
        },
        {
            "description": "Grammar with single subexpression and (0, 1) quantifier",
            "grammar": Grammar([String("a")], length_range=(0, 1)),
            "expected": Grammar([String("a")], length_range=(0, 1)),
        },
        {
            "description": "Grammar with consecutive String subexpressions",
            "grammar": Grammar([String("a"), String("b")]),
            "expected": String("ab"),
        },
        {
            "description": "Grammar with consecutive String subexpressions and (0, 1) quantifier",
            "grammar": Grammar([String("a"), String("b")], length_range=(0, 1)),
            "expected": Grammar([String("ab")], length_range=(0, 1)),
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
                    Grammar([String("a")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar([String("a")], length_range=(0, 1)),
        },
        {
            "description": "Nested Grammar with consecutive String subexpressions and (0, 1) quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], length_range=(0, 1)),
                    Grammar([String("c"), String("d")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar([String("ab")], length_range=(0, 1)),
                    Grammar([String("cd")], length_range=(0, 1)),
                ],
                length_range=(1, 1),
            ),
        },
        {
            "description": "Nested Grammar with consecutive String subexpressions and (0, 5) quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], length_range=(0, 5)),
                    Grammar([String("c"), String("d")], length_range=(0, 5)),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar([String("ab")], length_range=(0, 5)),
                    Grammar([String("cd")], length_range=(0, 5)),
                ],
                length_range=(1, 1),
            ),
        },
        {
            "description": "Nested Grammar with consecutive String subexpressions and (0, 5) quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], length_range=(0, 5)),
                    Grammar([String("c"), String("d")], length_range=(0, 5)),
                ],
                length_range=(0, 5),
            ),
            "expected": Grammar(
                [
                    Grammar([String("ab")], length_range=(0, 5)),
                    Grammar([String("cd")], length_range=(0, 5)),
                ],
                length_range=(1, 5),
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
            "expected": Grammar([Or([String("a"), String("b")])], length_range=(3, 3)),
        },
        {
            "description": "Grammar with simple repeating subexpression and (0, 1) quantifier",
            "grammar": Grammar(
                [
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                    Or([String("a"), String("b")]),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar(
                        [Or([String("a"), String("b")])],
                        length_range=(3, 3),
                    ),
                ],
                length_range=(0, 1),
            ),
        },
        {
            "description": "Grammar with simple repeating subexpression and (0, 1) quantifiers",
            "grammar": Grammar(
                [
                    Or([String("a"), String("b")], length_range=(0, 1)),
                    Or([String("a"), String("b")], length_range=(0, 1)),
                    Or([String("a"), String("b")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Or([String("a"), String("b")], length_range=(0, 3)),
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
                    Grammar([String("a"), String("b")], length_range=(0, 1)),
                    Grammar([String("a"), String("b")], length_range=(0, 1)),
                    Grammar([String("a"), String("b")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar([String("ab")], length_range=(0, 3)),
        },
        {
            "description": "Grammar with complex repeating subexpression and 0-n quantifiers",
            "grammar": Grammar(
                [
                    Grammar([String("a"), String("b")], length_range=(0, 3)),
                    Grammar([String("a"), String("b")], length_range=(0, 4)),
                    Grammar([String("a"), String("b")], length_range=(0, 5)),
                ],
                length_range=(0, 2),
            ),
            "expected": Grammar([String("ab")], length_range=(0, 2 * (3 + 4 + 5))),
        },
        {
            "description": "Grammar with complex repeating subexpressions",
            "grammar": Grammar(
                [
                    # Group 1
                    #   Group 1, Subgroup 1
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    #   Group 1, Subgroup 2
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    Grammar([String("baz")], length_range=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    #   Group 2, Subgroup 2
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    Grammar([String("baz")], length_range=(0, 1)),
                ],
            ),
            "expected": Grammar(
                [
                    Grammar(
                        [
                            Grammar([String("foo")], length_range=(0, 1)),
                            Grammar([String("bar")], length_range=(0, 1)),
                        ],
                        length_range=(2, 2),
                    ),
                    Grammar([String("baz")], length_range=(0, 1)),
                ],
                length_range=(2, 2),
            ),
        },
        {
            "description": "Grammar with complex repeating subexpressions and surrounding subexpressions",
            "grammar": Grammar(
                [
                    # Leading subexpression
                    Grammar([String("qux")], length_range=(0, 1)),
                    # Group 1
                    #   Group 1, Subgroup 1
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    #   Group 1, Subgroup 2
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    Grammar([String("baz")], length_range=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    #   Group 2, Subgroup 2
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    Grammar([String("baz")], length_range=(0, 1)),
                    # Trailing subexpression
                    Grammar([String("quux")], length_range=(0, 1)),
                ],
            ),
            "expected": Grammar(
                [
                    Grammar([String("qux")], length_range=(0, 1)),
                    Grammar(
                        [
                            Grammar(
                                [
                                    Grammar([String("foo")], length_range=(0, 1)),
                                    Grammar([String("bar")], length_range=(0, 1)),
                                ],
                                length_range=(2, 2),
                            ),
                            Grammar([String("baz")], length_range=(0, 1)),
                        ],
                        length_range=(2, 2),
                    ),
                    Grammar([String("quux")], length_range=(0, 1)),
                ],
            ),
        },
        {
            "description": "Grammar with complex repeating subexpressions and (0, 1) quantifier",
            "grammar": Grammar(
                [
                    # Group 1
                    #   Group 1, Subgroup 1
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    #   Group 1, Subgroup 2
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    Grammar([String("baz")], length_range=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    #   Group 2, Subgroup 2
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    Grammar([String("baz")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar(
                        [
                            Grammar(
                                [
                                    Grammar([String("foo")], length_range=(0, 1)),
                                    Grammar([String("bar")], length_range=(0, 1)),
                                ],
                                length_range=(2, 2),
                            ),
                            Grammar([String("baz")], length_range=(0, 1)),
                        ],
                        length_range=(2, 2),
                    ),
                ],
                length_range=(0, 1),
            ),
        },
        {
            "description": "Grammar with complex repeating subexpressions, (0, 1) quantifier, and surrounding subexpressions",
            "grammar": Grammar(
                [
                    # Leading subexpression
                    Grammar([String("qux")], length_range=(0, 1)),
                    # Group 1
                    #   Group 1, Subgroup 1
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    #   Group 1, Subgroup 2
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    Grammar([String("baz")], length_range=(0, 1)),
                    # Group 2
                    #   Group 2, Subgroup 1
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    #   Group 2, Subgroup 2
                    Grammar([String("foo")], length_range=(0, 1)),
                    Grammar([String("bar")], length_range=(0, 1)),
                    Grammar([String("baz")], length_range=(0, 1)),
                    # Trailing subexpression
                    Grammar([String("quux")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar(
                [
                    Grammar([String("qux")], length_range=(0, 1)),
                    Grammar(
                        [
                            Grammar(
                                [
                                    Grammar([String("foo")], length_range=(0, 1)),
                                    Grammar([String("bar")], length_range=(0, 1)),
                                ],
                                length_range=(2, 2),
                            ),
                            Grammar([String("baz")], length_range=(0, 1)),
                        ],
                        length_range=(2, 2),
                    ),
                    Grammar([String("quux")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
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
            "grammar": Or([], length_range=(0, 1)),
            "expected": None,
        },
        {
            "description": "Or with single subexpression",
            "grammar": Or([String("a")]),
            "expected": String("a"),
        },
        {
            "description": "Or with single subexpression and (0, 1) quantifier",
            "grammar": Or([String("a")], length_range=(0, 1)),
            "expected": Grammar([String("a")], length_range=(0, 1)),
        },
        {
            "description": "Or with consecutive String subexpressions",
            "grammar": Or([String("a"), String("b")]),
            "expected": Or([String("a"), String("b")]),
        },
        {
            "description": "Or with consecutive String subexpressions and (0, 1) quantifier",
            "grammar": Or([String("a"), String("b")], length_range=(0, 1)),
            "expected": Or([String("a"), String("b")], length_range=(0, 1)),
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
                    Or([String("a")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar([String("a")], length_range=(0, 1)),
        },
        {
            "description": "Nested Or with consecutive String subexpressions and (0, 1) quantifiers",
            "grammar": Or(
                [
                    Or([String("a"), String("b")], length_range=(0, 1)),
                    Or([String("c"), String("d")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Or(
                [
                    Or([String("a"), String("b")], length_range=(0, 1)),
                    Or([String("c"), String("d")], length_range=(0, 1)),
                ],
                length_range=(1, 1),
            ),
        },
        {
            "description": "Nested Or with consecutive String subexpressions and (0, 5) quantifiers",
            "grammar": Or(
                [
                    Or([String("a"), String("b")], length_range=(0, 5)),
                    Or([String("c"), String("d")], length_range=(0, 5)),
                ],
                length_range=(0, 5),
            ),
            "expected": Or(
                [
                    Or([String("a"), String("b")], length_range=(0, 5)),
                    Or([String("c"), String("d")], length_range=(0, 5)),
                ],
                length_range=(1, 5),
            ),
        },
        {
            "description": "Or with simple repeating subexpression",
            "grammar": Or([String("a"), String("a"), String("a")]),
            "expected": String("a"),
        },
        {
            "description": "Or with simple repeating subexpression and (0, 1) quantifier",
            "grammar": Or([String("a"), String("a"), String("a")], length_range=(0, 1)),
            "expected": Grammar([String("a")], length_range=(0, 1)),
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
                    Grammar([String("a"), String("b")], length_range=(0, 1)),
                    Grammar([String("a"), String("b")], length_range=(0, 1)),
                    Grammar([String("a"), String("b")], length_range=(0, 1)),
                ],
                length_range=(0, 1),
            ),
            "expected": Grammar([String("ab")], length_range=(0, 1)),
        },
    ],
)
def test_or_simplify(test_case):
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
                        length_range=(0, 1),
                    ),
                    Or(
                        [
                            Grammar([String("e"), String("f")]),
                            Grammar([String("g"), String("h")]),
                        ],
                        length_range=(0, 1),
                    ),
                ],
                length_range=(0, 1),
            ),
        ],
        length_range=(0, 1),
    )
    expected = Or(
        [
            Or([String("ab"), String("cd")], length_range=(0, 1)),
            Or([String("ef"), String("gh")], length_range=(0, 1)),
        ],
    )
    actual = grammar.simplify()
    assert actual == expected, "\n".join(
        (
            f"Grammar: {grammar_to_string(grammar)!s}",
            f"Expected: {grammar_to_string(expected)!s}",
            f"Actual: {grammar_to_string(actual)!s}",
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
                        length_range=(0, 1),
                    ),
                    Grammar(
                        [
                            Grammar([String("e"), String("f")]),
                            Grammar([String("g"), String("h")]),
                        ],
                        length_range=(0, 1),
                    ),
                ],
                length_range=(0, 1),
            ),
        ],
        length_range=(0, 1),
    )
    expected = Grammar(
        [
            Grammar([String("abcd")], length_range=(0, 1)),
            Grammar([String("efgh")], length_range=(0, 1)),
        ],
    )
    actual = grammar.simplify()
    assert actual == expected, "\n".join(
        (
            f"Grammar: {grammar_to_string(grammar)!s}",
            f"Expected: {grammar_to_string(expected)!s}",
            f"Actual: {grammar_to_string(actual)!s}",
        )
    )
