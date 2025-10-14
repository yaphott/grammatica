import pytest

from grammatica.grammar.string import String

try:
    from .helpers import NoOpGroupGrammar
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))
    from helpers import NoOpGroupGrammar


@pytest.mark.parametrize(
    "quantifier, expected",
    [
        (1, (1, 1)),
        (5, (5, 5)),
        ((1, 1), (1, 1)),
        ((0, 1), (0, 1)),
        ((0, 5), (0, 5)),
        ((0, None), (0, None)),
        ((1, None), (1, None)),
        ((5, 5), (5, 5)),
        ((2, 5), (2, 5)),
        ((2, None), (2, None)),
    ],
)
def test_base_group_grammar_quantifier(quantifier, expected):
    grammar = NoOpGroupGrammar([String("a")], quantifier=quantifier)
    assert grammar.quantifier == expected


def test_base_group_grammar_quantifier_validation_negative_lower_bound():
    with pytest.raises(ValueError, match=r"Range lower bound must be non-negative: \(-1, 5\)"):
        NoOpGroupGrammar([String("a")], quantifier=(-1, 5))


def test_base_group_grammar_quantifier_validation_zero_upper_bound():
    with pytest.raises(
        ValueError,
        match=r"Range upper bound must be positive or None \(infinity\): \(0, 0\)",
    ):
        NoOpGroupGrammar([String("a")], quantifier=(0, 0))


def test_base_group_grammar_quantifier_validation_negative_upper_bound():
    with pytest.raises(
        ValueError,
        match=r"Range upper bound must be positive or None \(infinity\): \(0, -1\)",
    ):
        NoOpGroupGrammar([String("a")], quantifier=(0, -1))


def test_base_group_grammar_quantifier_validation_lower_bound_greater_than_upper_bound():
    with pytest.raises(ValueError, match=r"Range lower bound must be <= range upper bound: \(5, 3\)"):
        NoOpGroupGrammar([], quantifier=(5, 3))


@pytest.mark.parametrize(
    "grammar, expected",
    [
        (NoOpGroupGrammar([]), None),
        (NoOpGroupGrammar([String("")]), None),
        (NoOpGroupGrammar([String("a"), String("")]), '"a"'),
    ],
)
def test_base_group_grammar_render(grammar, expected):
    actual = super(NoOpGroupGrammar, grammar).render()
    if expected is None:
        assert actual is None
    else:
        assert actual == expected


def test_base_group_grammar_simplify():
    grammar = NoOpGroupGrammar([])
    assert super(NoOpGroupGrammar, grammar).simplify() is None


def test_base_group_grammar_simplify_subexprs():
    grammar = NoOpGroupGrammar([])
    assert super(NoOpGroupGrammar, grammar).simplify_subexprs(grammar.subexprs, grammar.quantifier) is None


def test_base_group_grammar_needs_wrapped():
    grammar = NoOpGroupGrammar([])
    assert super(NoOpGroupGrammar, grammar).needs_wrapped() is False


def test_base_group_grammar_attrs_dict():
    grammar = NoOpGroupGrammar([])
    assert super(NoOpGroupGrammar, grammar).attrs_dict() == {
        "subexprs": [],
        "quantifier": (1, 1),
    }


@pytest.mark.parametrize(
    "quantifier, expected",
    [
        ((1, 1), None),
        ((0, 1), "?"),
        ((0, 2), "{0,2}"),
        ((0, None), "*"),
        ((1, None), "+"),
        ((2, 2), "{2}"),
        ((2, 5), "{2,5}"),
        ((2, None), "{2,}"),
    ],
)
def test_base_group_grammar_render_quantifier(quantifier, expected):
    grammar = NoOpGroupGrammar([], quantifier=quantifier)
    assert grammar.render_quantifier() == expected
