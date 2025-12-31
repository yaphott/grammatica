from grammatica.builder.json_.null import JSONNullLiteral
from grammatica.grammar.string import String

try:
    from ...helpers import fmt_result
except ImportError:
    import sys
    from os import path

    sys.path.append(path.dirname(path.dirname(path.dirname(path.abspath(__file__)))))
    from tests.helpers import fmt_result


def test_json_null_literal_grammar():
    component = JSONNullLiteral()
    expected = String("null")
    actual = component.grammar()
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(component)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_json_null_literal_attrs_dict():
    component = JSONNullLiteral()
    expected = {}
    actual = component.attrs_dict()
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(component)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_json_null_literal_as_string():
    component = JSONNullLiteral()
    expected = "JSONNullLiteral()"
    actual = repr(component)
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(component)!s}",
            f"Expected: {expected!s}",
            f"Actual: {actual!s}",
        )
    )
