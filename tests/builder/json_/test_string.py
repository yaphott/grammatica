import pytest

from grammatica.builder.json_.string import JSONString, JSONStringLiteral
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
            "description": "Empty",
            "value": "",
            "ensure_ascii": False,
            "expected": String('""'),
        },
        {
            "description": "ASCII characters",
            "value": "gandalf",
            "ensure_ascii": False,
            "expected": String('"gandalf"'),
        },
        {
            "description": "Non-ASCII characters with ensure_ascii=True",
            "value": "gandalf😀",
            "ensure_ascii": True,
            "expected": String('"gandalf\\ud83d\\ude00"'),
        },
        {
            "description": "Non-ASCII characters with ensure_ascii=False",
            "value": "gandalf😀",
            "ensure_ascii": False,
            "expected": String('"gandalf😀"'),
        },
    ],
)
def test_json_string_literal_grammar(test_case):
    component = JSONStringLiteral(test_case["value"], ensure_ascii=test_case["ensure_ascii"])
    actual = component.grammar()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Value: {fmt_result(test_case['value'])!s}",
            f"Ensure ASCII: {fmt_result(test_case['ensure_ascii'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_json_string_literal_attrs_dict():
    component = JSONStringLiteral("gandalf😀", ensure_ascii=False)
    expected = {"value": "gandalf😀", "ensure_ascii": False}
    actual = component.attrs_dict()
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(component)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Empty",
            "component": JSONStringLiteral(""),
            "expected": "JSONStringLiteral(value='', ensure_ascii=True)",
        },
        {
            "description": "ASCII characters",
            "component": JSONStringLiteral("gandalf"),
            "expected": "JSONStringLiteral(value='gandalf', ensure_ascii=True)",
        },
        {
            "description": "Non-ASCII characters",
            "component": JSONStringLiteral("gandalf😀", ensure_ascii=False),
            "expected": "JSONStringLiteral(value='gandalf😀', ensure_ascii=False)",
        },
    ],
)
def test_json_string_literal_as_string(test_case):
    actual = test_case["component"].as_string(indent=None)
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Component: {fmt_result(test_case['component'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


# def test_json_boolean_grammar():
#     component = JSONBoolean()
#     actual = component.grammar()
#     expected = Or([String("true"), String("false")])
#     assert actual == expected, "\n".join(
#         (
#             f"Component: {fmt_result(component)!s}",
#             f"Expected: {fmt_result(expected)!s}",
#             f"Actual: {fmt_result(actual)!s}",
#         )
#     )
#
#
# def test_json_boolean_attrs_dict():
#     component = JSONBoolean()
#     actual = component.attrs_dict()
#     expected = {}
#     assert actual == expected, "\n".join(
#         (
#             f"Component: {fmt_result(component)!s}",
#             f"Expected: {fmt_result(expected)!s}",
#             f"Actual: {fmt_result(actual)!s}",
#         )
#     )
#
#
# def test_json_boolean_as_string():
#     component = JSONBoolean()
#     actual = component.as_string(indent=None)
#     expected = "JSONBoolean()"
#     assert actual == expected, "\n".join(
#         (
#             f"Component: {fmt_result(component)!s}",
#             f"Expected: {fmt_result(expected)!s}",
#             f"Actual: {fmt_result(actual)!s}",
#         )
#     )
