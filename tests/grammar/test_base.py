import pytest

from grammatica.grammar.base import (
    BaseGrammar,
    BaseGroupGrammar,
    value_is_simple,
    value_to_string,
)
from grammatica.grammar.string import String

try:
    from .helpers import fmt_result
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))
    from helpers import fmt_result


class NoOpGrammar(BaseGrammar):
    def __init__(self):
        pass

    def render(self, wrap=False):
        return self.render()

    def simplify(self):
        return self.simplify()

    def attrs_dict(self):
        return {}


class NoOpGrammarAlt(BaseGrammar):
    def __init__(self):
        pass

    def render(self, wrap=False):
        return self.render()

    def simplify(self):
        return self.simplify()

    def attrs_dict(self):
        return {}


class NoOpGroupGrammar(BaseGroupGrammar):
    separator = " "

    def __init__(
        self,
        exprs,
        length_range=None,
    ):
        super().__init__(
            subexprs=exprs,
            length_range=length_range if length_range else (1, 1),
        )

    @staticmethod
    def simplify_subexprs(original_subexprs, length_range):
        return None

    def needs_wrapped(self):
        return False

    def attrs_dict(self):
        return super().attrs_dict()


def test_base_grammar_render():
    grammar = NoOpGrammar()
    assert super(NoOpGrammar, grammar).render() is None


def test_base_grammar_simplify():
    grammar = NoOpGrammar()
    assert super(NoOpGrammar, grammar).simplify() is None


def test_base_grammar_attrs_dict():
    grammar = NoOpGrammar()
    assert super(NoOpGrammar, grammar).attrs_dict() == {}


@pytest.mark.parametrize(
    "length_range, expected",
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
def test_base_group_grammar_quantifier(length_range, expected):
    grammar = NoOpGroupGrammar([String("a")], length_range=length_range)
    assert grammar.length_range == expected


def test_base_group_grammar_quantifier_validation_negative_lower_bound():
    with pytest.raises(ValueError, match=r"Range lower bound must be non-negative: \(-1, 5\)"):
        NoOpGroupGrammar([String("a")], length_range=(-1, 5))


def test_base_group_grammar_quantifier_validation_zero_upper_bound():
    with pytest.raises(
        ValueError,
        match=r"Range upper bound must be positive or None \(infinity\): \(0, 0\)",
    ):
        NoOpGroupGrammar([String("a")], length_range=(0, 0))


def test_base_group_grammar_quantifier_validation_negative_upper_bound():
    with pytest.raises(
        ValueError,
        match=r"Range upper bound must be positive or None \(infinity\): \(0, -1\)",
    ):
        NoOpGroupGrammar([String("a")], length_range=(0, -1))


def test_base_group_grammar_quantifier_validation_lower_bound_greater_than_upper_bound():
    with pytest.raises(ValueError, match=r"Range lower bound must be <= range upper bound: \(5, 3\)"):
        NoOpGroupGrammar([], length_range=(5, 3))


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
    assert super(NoOpGroupGrammar, grammar).simplify_subexprs(grammar.subexprs, grammar.length_range) is None


def test_base_group_grammar_needs_wrapped():
    grammar = NoOpGroupGrammar([])
    assert super(NoOpGroupGrammar, grammar).needs_wrapped() is False


def test_base_group_grammar_attrs_dict():
    grammar = NoOpGroupGrammar([])
    assert super(NoOpGroupGrammar, grammar).attrs_dict() == {
        "subexprs": [],
        "length_range": (1, 1),
    }


@pytest.mark.parametrize(
    "length_range, expected",
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
def test_base_group_grammar_render_quantifier(length_range, expected):
    grammar = NoOpGroupGrammar([], length_range=length_range)
    assert grammar.render_quantifier() == expected


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Not equal to non-grammar type: None",
            "grammar": NoOpGrammar(),
            "other": None,
            "check_length_range": True,
            "expected": False,
        },
        {
            "description": "Not equal to non-grammar type: str",
            "grammar": NoOpGrammar(),
            "other": "test",
            "check_length_range": True,
            "expected": False,
        },
        {
            "description": "Not equal to non-grammar type: int",
            "grammar": NoOpGrammar(),
            "other": 42,
            "check_length_range": True,
            "expected": False,
        },
        {
            "description": "Not equal to non-grammar type: float",
            "grammar": NoOpGrammar(),
            "other": 3.14,
            "check_length_range": True,
            "expected": False,
        },
        {
            "description": "Equal to same type and same attributes",
            "grammar": NoOpGrammar(),
            "other": NoOpGrammar(),
            "check_length_range": True,
            "expected": True,
        },
        {
            "description": "Equal to same type and attributes",
            "grammar": NoOpGroupGrammar([String("a")], length_range=(1, 1)),
            "other": NoOpGroupGrammar([String("a")], length_range=(1, 1)),
            "check_length_range": True,
            "expected": True,
        },
        {
            "description": "Not equal to same type and different attributes",
            "grammar": NoOpGroupGrammar([String("a")], length_range=(1, 1)),
            "other": NoOpGroupGrammar([String("b")], length_range=(1, 1)),
            "check_length_range": True,
            "expected": False,
        },
        {
            "description": "Equal to different grammar with the same attributes",
            "grammar": NoOpGrammar(),
            "other": NoOpGrammarAlt(),
            "check_length_range": True,
            "expected": True,
        },
        {
            "description": "Not equal when length_range differs and check_length_range is True",
            "grammar": NoOpGroupGrammar([String("a")], length_range=(1, 1)),
            "other": NoOpGroupGrammar([String("a")], length_range=(2, 2)),
            "check_length_range": True,
            "expected": False,
        },
        {
            "description": "Equal when length_range differs and check_length_range is False",
            "grammar": NoOpGroupGrammar([String("a")], length_range=(1, 1)),
            "other": NoOpGroupGrammar([String("a")], length_range=(2, 2)),
            "check_length_range": False,
            "expected": True,
        },
        {
            "description": "Different String instances",
            "grammar": String("test"),
            "other": String("different"),
            "check_length_range": False,
            "expected": False,
        },
    ],
)
def test_base_grammar_equals(test_case):
    actual = test_case["grammar"].equals(
        test_case["other"],
        check_length_range=test_case["check_length_range"],
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


def test_base_grammar_equals_same_instance():
    grammar = NoOpGrammar()
    assert grammar.equals(grammar)


@pytest.mark.parametrize(
    "value, expected",
    [
        (None, True),
        (42, True),
        (3.14, True),
        ("string", True),
        (True, True),
        (False, True),
        ([], False),
        (tuple(), False),
        ({}, False),
        (set(), False),
        (frozenset(), False),
        (String("test"), False),
    ],
)
def test_value_is_simple(value, expected):
    assert value_is_simple(value) == expected


@pytest.mark.parametrize(
    "value, indent, expected",
    [
        # None
        (None, None, "None"),
        # Boolean
        (False, None, "False"),
        (True, None, "True"),
        # Number
        (0, None, "0"),
        (42, None, "42"),
        (0.0, None, "0.0"),
        (3.14, None, "3.14"),
        # String
        ("", None, "''"),
        ("test", None, "'test'"),
        # List
        ([], None, "[]"),
        ([], 2, "[]"),
        ([1, 2], None, "[1, 2]"),
        ([1, 2], 2, "[1, 2]"),
        # Tuple
        (tuple(), None, "()"),
        (tuple(), 2, "()"),
        ((1, 2), None, "(1, 2)"),
        ((1, 2), 2, "(1, 2)"),
        # Set
        (set(), None, "set()"),
        (set(), 2, "set()"),
        ({1, 2}, None, "{1, 2}"),
        ({1, 2}, 2, "{1, 2}"),
        (frozenset(), None, "frozenset()"),
        (frozenset(), 2, "frozenset()"),
        (frozenset({1, 2}), None, "frozenset({1, 2})"),
        (frozenset({1, 2}), 2, "frozenset({1, 2})"),
        # Dict
        ({}, None, "{}"),
        ({}, 2, "{}"),
        ({"key": "value"}, None, "{'key': 'value'}"),
        # Grammar
        (String("a"), None, "String(value='a')"),
        ([String("a"), String("b")], None, "[String(value='a'), String(value='b')]"),
        ([String("a"), String("b")], 2, "[\n  String(value='a'),\n  String(value='b')\n]"),
        ((String("a"), String("b")), None, "(String(value='a'), String(value='b'))"),
        ((String("a"), String("b")), 2, "(\n  String(value='a'),\n  String(value='b')\n)"),
        (
            {"a": String("a"), "b": String("b")},
            None,
            "{'a': String(value='a'), 'b': String(value='b')}",
        ),
        (
            {"a": String("a"), "b": String("b")},
            2,
            "{\n  'a': String(value='a'),\n  'b': String(value='b')\n}",
        ),
    ],
)
def test_value_to_string(value, indent, expected):
    assert value_to_string(value, indent=indent) == expected


def test_value_to_string_unsupported_type():
    class CustomType:
        pass

    with pytest.raises(ValueError, match=r"Unsupported value type: CustomType"):
        value_to_string(CustomType(), indent=None)


# TODO: More direct way to test would be to have a child of BaseGrammar that defines some attrs
@pytest.mark.parametrize(
    "grammar, indent, expected",
    [
        (
            NoOpGroupGrammar([String("a")]),
            None,
            "NoOpGroupGrammar(subexprs=[String(value='a')], length_range=(1, 1))",
        ),
        (
            NoOpGroupGrammar([String("a")]),
            2,
            "NoOpGroupGrammar(\n  subexprs=[\n    String(value='a')\n  ],\n  length_range=(1, 1)\n)",
        ),
    ],
)
def test_base_grammar_as_string(grammar, indent, expected):
    assert grammar.as_string(indent=indent) == expected


def test_base_grammar_str_and_repr():
    grammar = NoOpGroupGrammar([String("a"), String("b")])
    assert (
        str(grammar)
        == repr(grammar)
        == grammar.as_string(indent=None)
        == "NoOpGroupGrammar(subexprs=[String(value='a'), String(value='b')], length_range=(1, 1))"
    )
