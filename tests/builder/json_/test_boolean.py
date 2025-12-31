import pytest

from grammatica.builder.json_.boolean import JSONBoolean, JSONBooleanLiteral
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

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
            "description": "Literal true",
            "value": True,
            "expected": String("true"),
        },
        {
            "description": "Literal false",
            "value": False,
            "expected": String("false"),
        },
    ],
)
def test_json_boolean_literal_grammar(test_case):
    component = JSONBooleanLiteral(test_case["value"])
    actual = component.grammar()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Component: {fmt_result(test_case['component'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Literal true",
            "value": True,
            "expected": {"value": True},
        },
        {
            "description": "Literal false",
            "value": False,
            "expected": {"value": False},
        },
    ],
)
def test_json_boolean_literal_attrs_dict(test_case):
    component = JSONBooleanLiteral(test_case["value"])
    actual = component.attrs_dict()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Component: {fmt_result(test_case['component'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Literal true",
            "component": JSONBooleanLiteral(True),
            "expected": "JSONBooleanLiteral(value=True)",
        },
        {
            "description": "Literal false",
            "component": JSONBooleanLiteral(False),
            "expected": "JSONBooleanLiteral(value=False)",
        },
    ],
)
def test_json_boolean_literal_as_string(test_case):
    actual = test_case["component"].as_string(indent=None)
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Component: {fmt_result(test_case['component'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_json_boolean_grammar():
    component = JSONBoolean()
    actual = component.grammar()
    expected = Or([String("true"), String("false")])
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(component)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_json_boolean_attrs_dict():
    component = JSONBoolean()
    actual = component.attrs_dict()
    expected = {}
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(component)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_json_boolean_as_string():
    component = JSONBoolean()
    actual = component.as_string(indent=None)
    expected = "JSONBoolean()"
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(component)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )
