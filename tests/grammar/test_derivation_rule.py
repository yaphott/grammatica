import pytest

from grammatica.grammar import DerivationRule, String
from grammatica.grammar.group import And, Or

try:
    from ..helpers import fmt_result
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))
    from tests.helpers import fmt_result


def test_derivation_rule_case_insensitive_symbol():
    rule = DerivationRule("Test-Symbol", String("value"))
    assert rule.symbol == "test-symbol"


def test_derivation_rule_validation_empty_symbol():
    with pytest.raises(ValueError, match="Derivation rule symbol cannot be empty"):
        DerivationRule("", String("value"))


def test_derivation_rule_validation_invalid_symbol_prefix():
    with pytest.raises(
        ValueError,
        match="Derivation rule symbol must start with an alphabetic character \\(a-z, A-Z\\)",
    ):
        DerivationRule("1invalid", String("value"))


def test_derivation_rule_validation_invalid_symbol_characters():
    with pytest.raises(
        ValueError,
        match="Derivation rule symbol must contain only alphanumeric characters \\(a-z, A-Z, 0-9\\) and hyphens \\(-\\) after the first character",
    ):
        DerivationRule("invalid_symbol", String("value"))


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
    symbol = "test-symbol"
    grammar = String("test_value")
    rule = DerivationRule(symbol, grammar)
    actual = rule.attrs_dict()
    expected = {
        "symbol": symbol,
        "value": grammar,
    }
    assert actual == expected
    assert actual["value"] is grammar


@pytest.mark.parametrize(
    "grammar, indent, expected",
    [
        (
            DerivationRule(
                "bool",
                Or([String("true"), String("false")]),
            ),
            None,
            "DerivationRule(symbol='bool', value=Or(subexprs=[String(value='true'), String(value='false')], quantifier=(1, 1)))",
        ),
        (
            DerivationRule("bool", Or([String("true"), String("false")])),
            2,
            "DerivationRule(\n  symbol='bool',\n  value=Or(\n    subexprs=[\n      String(value='true'),\n      String(value='false')\n    ],\n    quantifier=(1, 1)\n  )\n)",
        ),
    ],
)
def test_derivation_rule_as_string(grammar, indent, expected):
    assert grammar.as_string(indent=indent) == expected
