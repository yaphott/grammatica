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
        # Tuple
        (tuple(), None, "()"),
        (tuple(), 2, "()"),
        ((42,), None, "(42,)"),
        ((42,), 2, "(42,)"),
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
            "{'a': String(value='a'), 'b': String(value='b')}"
        ),
        (
            {"a": String("a"), "b": String("b")},
            2,
            "{\n  'a': String(value='a'),\n  'b': String(value='b')\n}"
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
