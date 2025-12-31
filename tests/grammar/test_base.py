import pytest

from grammatica.grammar import String

try:
    from ..helpers import (
        NoOpGrammar,
        NoOpGrammarAlt,
        NoOpGroupGrammar,
        fmt_result,
    )
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))
    from tests.helpers import (
        NoOpGrammar,
        NoOpGrammarAlt,
        NoOpGroupGrammar,
        fmt_result,
    )


def test_grammar_render():
    grammar = NoOpGrammar()
    assert super(NoOpGrammar, grammar).render() is None


def test_grammar_simplify():
    grammar = NoOpGrammar()
    assert super(NoOpGrammar, grammar).simplify() is None


def test_grammar_attrs_dict():
    grammar = NoOpGrammar()
    assert super(NoOpGrammar, grammar).attrs_dict() == {}


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Not equal to non-grammar type: None",
            "grammar": NoOpGrammar(),
            "other": None,
            "check_quantifier": True,
            "expected": False,
        },
        {
            "description": "Not equal to non-grammar type: str",
            "grammar": NoOpGrammar(),
            "other": "test",
            "check_quantifier": True,
            "expected": False,
        },
        {
            "description": "Not equal to non-grammar type: int",
            "grammar": NoOpGrammar(),
            "other": 42,
            "check_quantifier": True,
            "expected": False,
        },
        {
            "description": "Not equal to non-grammar type: float",
            "grammar": NoOpGrammar(),
            "other": 3.14,
            "check_quantifier": True,
            "expected": False,
        },
        {
            "description": "Equal to same type and same attributes",
            "grammar": NoOpGrammar(),
            "other": NoOpGrammar(),
            "check_quantifier": True,
            "expected": True,
        },
        {
            "description": "Equal to same type and attributes",
            "grammar": NoOpGroupGrammar([String("a")], quantifier=(1, 1)),
            "other": NoOpGroupGrammar([String("a")], quantifier=(1, 1)),
            "check_quantifier": True,
            "expected": True,
        },
        {
            "description": "Not equal to same type and different attributes",
            "grammar": NoOpGroupGrammar([String("a")], quantifier=(1, 1)),
            "other": NoOpGroupGrammar([String("b")], quantifier=(1, 1)),
            "check_quantifier": True,
            "expected": False,
        },
        {
            "description": "Not equal to different grammar type even with the same attributes",
            "grammar": NoOpGrammar(),
            "other": NoOpGrammarAlt(),
            "check_quantifier": True,
            "expected": False,
        },
        {
            "description": "Not equal when quantifier differs and check_quantifier is True",
            "grammar": NoOpGroupGrammar([String("a")], quantifier=(1, 1)),
            "other": NoOpGroupGrammar([String("a")], quantifier=(2, 2)),
            "check_quantifier": True,
            "expected": False,
        },
        {
            "description": "Equal when quantifier differs and check_quantifier is False",
            "grammar": NoOpGroupGrammar([String("a")], quantifier=(1, 1)),
            "other": NoOpGroupGrammar([String("a")], quantifier=(2, 2)),
            "check_quantifier": False,
            "expected": True,
        },
        {
            "description": "Different String instances",
            "grammar": String("test"),
            "other": String("different"),
            "check_quantifier": False,
            "expected": False,
        },
    ],
)
def test_grammar_equals(test_case):
    actual = test_case["grammar"].equals(
        test_case["other"],
        check_quantifier=test_case["check_quantifier"],
    )
    assert actual is test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Grammar: {fmt_result(test_case['grammar'])!s}",
            f"Other: {fmt_result(test_case['other'])!s}",
            f"Expected: {test_case['expected']!s}",
            f"Actual: {actual!s}",
        )
    )


def test_grammar_equals_same_instance():
    grammar = NoOpGrammar()
    assert grammar == grammar


# TODO: More direct way to test would be to have a child of Grammar that defines some attrs
@pytest.mark.parametrize(
    "grammar, indent, expected",
    [
        (
            NoOpGroupGrammar([String("a")]),
            None,
            "NoOpGroupGrammar(subexprs=[String(value='a')], quantifier=(1, 1))",
        ),
        (
            NoOpGroupGrammar([String("a")]),
            2,
            "NoOpGroupGrammar(\n  subexprs=[\n    String(value='a')\n  ],\n  quantifier=(1, 1)\n)",
        ),
    ],
)
def test_grammar_as_string(grammar, indent, expected):
    assert grammar.as_string(indent=indent) == expected


def test_grammar_str_and_repr():
    grammar = NoOpGroupGrammar([String("a"), String("b")])
    assert (
        str(grammar)
        == repr(grammar)
        == grammar.as_string(indent=None)
        == "NoOpGroupGrammar(subexprs=[String(value='a'), String(value='b')], quantifier=(1, 1))"
    )
