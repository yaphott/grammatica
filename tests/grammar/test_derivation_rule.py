import pytest

from grammatica.grammar.derivation_rule import DerivationRule
from grammatica.grammar.group.and_ import And
from grammatica.grammar.group.or_ import Or
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
            "description": "Simple derivation rule",
            "grammar": DerivationRule("symbol", String("value")),
            "expected": 'symbol ::= "value"',
        },
        {
            "description": "Empty value returns None",
            "grammar": DerivationRule("symbol", String("")),
            "expected": None,
        },
        {
            "description": "With Grammar",
            "grammar": DerivationRule("expr", And([String("a"), String("b")])),
            "expected": 'expr ::= "a" "b"',
        },
        {
            "description": "With Or",
            "grammar": DerivationRule("choice", Or([String("a"), String("b")])),
            "expected": 'choice ::= ("a" | "b")',
        },
    ],
)
def test_derivation_rule_render_full(test_case):
    actual = test_case["grammar"].render(full=True)
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
            "description": "Returns symbol when full=False",
            "grammar": DerivationRule("symbol", String("value")),
            "expected": "symbol",
        },
        {
            "description": "Returns symbol even with empty value",
            "grammar": DerivationRule("empty", String("")),
            "expected": "empty",
        },
    ],
)
def test_derivation_rule_render_not_full(test_case):
    actual = test_case["grammar"].render(full=False)
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
            "description": "Wrap Or with multiple subexpressions when wrap=True",
            "grammar": DerivationRule("choice", Or([String("a"), String("b")])),
            "wrap": True,
            "expected": 'choice ::= ("a" | "b")',
        },
        {
            "description": "Do not wrap Or with single subexpression when wrap=True",
            "grammar": DerivationRule("choice", Or([String("a")])),
            "wrap": True,
            "expected": 'choice ::= "a"',
        },
        {
            "description": "Do not wrap Or with multiple subexpressions when wrap=False",
            "grammar": DerivationRule("choice", Or([String("a"), String("b")])),
            "wrap": False,
            "expected": 'choice ::= "a" | "b"',
        },
        {
            "description": "Do not wrap Or with single subexpression when wrap=False",
            "grammar": DerivationRule("choice", Or([String("a")])),
            "wrap": False,
            "expected": 'choice ::= "a"',
        },
        {
            "description": "Do not wrap And when wrap=True",
            "grammar": DerivationRule("choice", And([String("a"), String("b")])),
            "wrap": True,
            "expected": 'choice ::= "a" "b"',
        },
        {
            "description": "Do not wrap when wrap=False",
            "grammar": DerivationRule("choice", Or([String("a"), String("b")])),
            "wrap": False,
            "expected": 'choice ::= "a" | "b"',
        },
    ],
)
def test_derivation_rule_render_wrap(test_case):
    actual = test_case["grammar"].render(full=True, wrap=test_case["wrap"])
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
            "description": "Simplifies value",
            "grammar": DerivationRule("merged", And([String("a"), String("b")])),
            "expected": DerivationRule("merged", String("ab")),
        },
        {
            "description": "Grammar that simplifies to None returns None",
            "grammar": DerivationRule("empty", And([])),
            "expected": None,
        },
    ],
)
def test_derivation_rule_simplify(test_case):
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


def test_derivation_rule_attrs_dict():
    symbol = "test_symbol"
    grammar = String("test_value")
    rule = DerivationRule(symbol, grammar)
    actual = rule.attrs_dict()
    expected = {
        "symbol": symbol,
        "value": grammar,
    }
    assert actual == expected
    assert actual["value"] is grammar
