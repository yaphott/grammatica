import pytest

from grammatica.grammar.string import String
from grammatica.utils import value_to_string

try:
    from ..helpers import fmt_result
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.abspath(__file__))))
    from tests.helpers import fmt_result


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
        ([42], None, "[42]"),
        ([42], 2, "[42]"),
        ([1, 2], None, "[1, 2]"),
        ([1, 2], 2, "[1, 2]"),
        ([1, [2], 3], None, "[1, [2], 3]"),
        ([1, [2], 3], 2, "[\n  1,\n  [2],\n  3\n]"),
        (
            [0, [[1]], 2, 3],
            None,
            "[0, [[1]], 2, 3]",
        ),
        (
            [0, [[1]], 2, 3],
            2,
            "[\n  0,\n  [\n    [1]\n  ],\n  2,\n  3\n]",
        ),
        (
            [None, False, 42, 4.2, "test"],
            None,
            "[None, False, 42, 4.2, 'test']",
        ),
        (
            [None, False, 42, 4.2, "test"],
            2,
            "[None, False, 42, 4.2, 'test']",
        ),
        # Tuple
        (tuple(), None, "()"),
        (tuple(), 2, "()"),
        ((42,), None, "(42,)"),
        ((42,), 2, "(42,)"),
        ((1, 2), None, "(1, 2)"),
        ((1, 2), 2, "(1, 2)"),
        ((1, (2,), 3), None, "(1, (2,), 3)"),
        ((1, (2,), 3), 2, "(\n  1,\n  (2,),\n  3\n)"),
        (
            (0, ((1,),), 2, 3),
            None,
            "(0, ((1,),), 2, 3)",
        ),
        (
            (0, ((1,),), 2, 3),
            2,
            "(\n  0,\n  (\n    (1,),\n  ),\n  2,\n  3\n)",
        ),
        (
            (None, False, 42, 4.2, "test"),
            None,
            "(None, False, 42, 4.2, 'test')",
        ),
        (
            (None, False, 42, 4.2, "test"),
            2,
            "(None, False, 42, 4.2, 'test')",
        ),
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
        (
            [String("a"), String("b")],
            None,
            "[String(value='a'), String(value='b')]",
        ),
        (
            [String("a"), String("b")],
            2,
            "[\n  String(value='a'),\n  String(value='b')\n]",
        ),
        (
            (String("a"), String("b")),
            None,
            "(String(value='a'), String(value='b'))",
        ),
        (
            (String("a"), String("b")),
            2,
            "(\n  String(value='a'),\n  String(value='b')\n)",
        ),
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
    actual = value_to_string(value, indent=indent)
    assert actual == expected, "\n".join(
        (
            f"Value: {fmt_result(value)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_value_to_string_unsupported_type():
    class CustomType:
        pass

    with pytest.raises(ValueError, match=r"Unsupported value type: CustomType"):
        value_to_string(CustomType(), indent=None)
