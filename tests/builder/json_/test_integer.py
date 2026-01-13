"""Tests for JSON integer composition classes."""

import pytest

from grammatica.builder.json_.integer import JSONInteger, JSONIntegerLiteral
from grammatica.grammar.char_range import CharRange
from grammatica.grammar.group.and_ import And
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
            "description": "Large negative",
            "value": -1234567890,
            "expected": String("-1234567890"),
        },
        {
            "description": "Negative",
            "value": -42,
            "expected": String("-42"),
        },
        {
            "description": "Zero",
            "value": 0,
            "expected": String("0"),
        },
        {
            "description": "Positive",
            "value": 42,
            "expected": String("42"),
        },
        {
            "description": "Large positive",
            "value": 1234567890,
            "expected": String("1234567890"),
        },
    ],
)
def test_json_integer_literal_grammar(test_case):
    component = JSONIntegerLiteral(test_case["value"])
    actual = component.grammar()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Value: {test_case['value']!r}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Negative integer literal",
            "value": -42,
            "expected": {"value": -42},
        },
        {
            "description": "Zero literal",
            "value": 0,
            "expected": {"value": 0},
        },
        {
            "description": "Positive integer literal",
            "value": 42,
            "expected": {"value": 42},
        },
    ],
)
def test_json_integer_literal_attrs_dict(test_case):
    component = JSONIntegerLiteral(test_case["value"])
    actual = component.attrs_dict()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Value: {test_case['value']!r}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


@pytest.mark.parametrize(
    "test_case",
    [
        {
            "description": "Negative integer literal",
            "component": JSONIntegerLiteral(-42),
            "expected": "JSONIntegerLiteral(value=-42)",
        },
        {
            "description": "Zero literal",
            "component": JSONIntegerLiteral(0),
            "expected": "JSONIntegerLiteral(value=0)",
        },
        {
            "description": "Positive integer literal",
            "component": JSONIntegerLiteral(42),
            "expected": "JSONIntegerLiteral(value=42)",
        },
    ],
)
def test_json_integer_literal_as_string(test_case):
    actual = test_case["component"].as_string(indent=None)
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
            "description": "Boundless: (-infinity, infinity)",
            "component": JSONInteger(minval=None, maxval=None),
            "expected": Or(
                [
                    And(
                        [
                            And([String("-")], quantifier=(0, 1)),
                            CharRange([("1", "9")]),
                            And([CharRange([("0", "9")])], quantifier=(0, None)),
                        ]
                    ),
                    String("0"),
                ]
            ),
        },
        {
            "description": "Unbounded max from negative: [-10, infinity)",
            "component": JSONInteger(minval=-10, maxval=None),
            "expected": Or(
                [
                    And(
                        [
                            String("-"),
                            Or(
                                [
                                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(2, None))]),
                                    Or(
                                        [
                                            And([String("9"), CharRange([("0", "9")])]),
                                            And([CharRange([("2", "8")]), CharRange([("0", "9")])]),
                                            And([String("1"), CharRange([("0", "9")])]),
                                        ],
                                    ),
                                ],
                            ),
                        ],
                    ),
                    String("0"),
                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(0, None))]),
                ],
            ),
        },
        {
            "description": "Unbounded max from negative: [-5, infinity)",
            "component": JSONInteger(minval=-5, maxval=None),
            "expected": Or(
                [
                    And(
                        [
                            String("-"),
                            Or(
                                [
                                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(1, None))]),
                                    CharRange([("5", "9")]),
                                ]
                            ),
                        ]
                    ),
                    String("0"),
                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(0, None))]),
                ]
            ),
        },
        {
            "description": "Unbounded max from zero: [0, infinity)",
            "component": JSONInteger(minval=0, maxval=None),
            "expected": Or(
                [
                    String("0"),
                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(0, None))]),
                ]
            ),
        },
        {
            "description": "Unbounded max from one: [5, infinity)",
            "component": JSONInteger(minval=5, maxval=None),
            "expected": Or(
                [
                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(1, None))]),
                    CharRange([("5", "9")]),
                ]
            ),
        },
        {
            "description": "Unbounded max from one: [10, infinity)",
            "component": JSONInteger(minval=10, maxval=None),
            "expected": Or(
                [
                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(2, None))]),
                    Or(
                        [
                            And([String("9"), CharRange([("0", "9")])]),
                            And([CharRange([("2", "8")]), CharRange([("0", "9")])]),
                            And([String("1"), CharRange([("0", "9")])]),
                        ],
                    ),
                ],
            ),
        },
        {
            "description": "Unbounded max from positive (where 1 < min < 10): [5, infinity)",
            "component": JSONInteger(minval=5, maxval=None),
            "expected": Or(
                [
                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(1, None))]),
                    CharRange([("5", "9")]),
                ]
            ),
        },
        {
            "description": "Unbounded max from positive (where min > 9): [10, infinity)",
            "component": JSONInteger(minval=10, maxval=None),
            "expected": Or(
                [
                    And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(2, None))]),
                    Or(
                        [
                            And([String("9"), CharRange([("0", "9")])]),
                            And([CharRange([("2", "8")]), CharRange([("0", "9")])]),
                            And([String("1"), CharRange([("0", "9")])]),
                        ],
                    ),
                ],
            ),
        },
        {
            "description": "Unbounded min to negative (where max < 0): (-infinity, -10]",
            "component": JSONInteger(minval=None, maxval=-10),
            "expected": And(
                [
                    String("-"),
                    Or(
                        [
                            And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(2, None))]),
                            Or(
                                [
                                    And([String("9"), CharRange([("0", "9")])]),
                                    And([CharRange([("2", "8")]), CharRange([("0", "9")])]),
                                    And([String("1"), CharRange([("0", "9")])]),
                                ],
                            ),
                        ],
                    ),
                ],
            ),
        },
        {
            "description": "Unbounded min to negative (where max < 0): (-infinity, -5]",
            "component": JSONInteger(minval=None, maxval=-5),
            "expected": And(
                [
                    String("-"),
                    Or(
                        [
                            And([CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(1, None))]),
                            CharRange([("5", "9")]),
                        ]
                    ),
                ]
            ),
        },
        {
            "description": "Unbounded min to zero: (-infinity, 0]",
            "component": JSONInteger(minval=None, maxval=0),
            "expected": Or(
                [
                    And([String("-"), CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(0, None))]),
                    String("0"),
                ]
            ),
        },
        {
            "description": "Unbounded min to positive (where max > 0): (-infinity, 5]",
            "component": JSONInteger(minval=None, maxval=5),
            "expected": Or(
                [
                    And([String("-"), CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(0, None))]),
                    String("0"),
                    CharRange([("1", "5")]),
                ]
            ),
        },
        {
            "description": "Unbounded min to positive (where max > 0): (-infinity, 10]",
            "component": JSONInteger(minval=None, maxval=10),
            "expected": Or(
                [
                    And([String("-"), CharRange([("1", "9")]), And([CharRange([("0", "9")])], quantifier=(0, None))]),
                    String("0"),
                    Or([String("10"), CharRange([("1", "9")])]),
                ],
            ),
        },
        {
            "description": "Bounded negative singe-digit: [-5, -1]",
            "component": JSONInteger(minval=-5, maxval=-1),
            "expected": And([String("-"), CharRange([("1", "5")])]),
        },
        {
            "description": "Bounded positive singe-digit: [1, 5]",
            "component": JSONInteger(minval=1, maxval=5),
            "expected": CharRange([("1", "5")]),
        },
        {
            "description": "Bounded spanning zero: [-5, 5]",
            "component": JSONInteger(minval=-5, maxval=5),
            "expected": Or(
                [
                    And([String("-"), CharRange([("1", "5")])]),
                    String("0"),
                    CharRange([("1", "5")]),
                ]
            ),
        },
        {
            "description": "Bounded spanning zero: [-100, 0]",
            "component": JSONInteger(minval=-100, maxval=0),
            "expected": Or(
                [
                    And(
                        [
                            String("-"),
                            Or(
                                [
                                    String("100"),
                                    And([CharRange([("1", "9")]), CharRange([("0", "9")])]),
                                    CharRange([("1", "9")]),
                                ]
                            ),
                        ]
                    ),
                    String("0"),
                ]
            ),
        },
        {
            "description": "Bounded spanning zero: [-100, 5]",
            "component": JSONInteger(minval=-100, maxval=5),
            "expected": Or(
                [
                    And(
                        [
                            String("-"),
                            Or(
                                [
                                    String("100"),
                                    And([CharRange([("1", "9")]), CharRange([("0", "9")])]),
                                    CharRange([("1", "9")]),
                                ]
                            ),
                        ]
                    ),
                    String("0"),
                    CharRange([("1", "5")]),
                ]
            ),
        },
        {
            "description": "Bounded spanning zero: [-5, 100]",
            "component": JSONInteger(minval=-5, maxval=100),
            "expected": Or(
                [
                    And([String("-"), CharRange([("1", "5")])]),
                    String("0"),
                    Or(
                        [
                            String("100"),
                            And([CharRange([("1", "9")]), CharRange([("0", "9")])]),
                            CharRange([("1", "9")]),
                        ]
                    ),
                ]
            ),
        },
        {
            "description": "Bounded spanning zero: [0, 100]",
            "component": JSONInteger(minval=0, maxval=100),
            "expected": Or(
                [
                    String("100"),
                    And([CharRange([("1", "9")]), CharRange([("0", "9")])]),
                    CharRange([("1", "9")]),
                    String("0"),
                ]
            ),
        },
        {
            "description": "Bounded negative (where min == max): [-42, -42]",
            "component": JSONInteger(minval=-42, maxval=-42),
            "expected": And([String("-"), String("42")]),
        },
        {
            "description": "Bounded negative (where min != max): [-42, -5]",
            "component": JSONInteger(minval=-42, maxval=-5),
            "expected": And(
                [
                    String("-"),
                    Or(
                        [
                            Or(
                                [
                                    And([String("4"), CharRange([("0", "2")])]),
                                    And([CharRange([("2", "3")]), CharRange([("0", "9")])]),
                                    And([String("1"), CharRange([("0", "9")])]),
                                ],
                            ),
                            CharRange([("5", "9")]),
                        ],
                    ),
                ],
            ),
        },
        {
            "description": "Bounded zero (where min == max): [0, 0]",
            "component": JSONInteger(minval=0, maxval=0),
            "expected": String("0"),
        },
        {
            "description": "Bounded positive (where min == max): [42, 42]",
            "component": JSONInteger(minval=42, maxval=42),
            "expected": String("42"),
        },
        {
            "description": "Bounded positive (where min != max): [5, 42]",
            "component": JSONInteger(minval=5, maxval=42),
            "expected": Or(
                [
                    Or(
                        [
                            And([String("4"), CharRange([("0", "2")])]),
                            And([CharRange([("2", "3")]), CharRange([("0", "9")])]),
                            And([String("1"), CharRange([("0", "9")])]),
                        ]
                    ),
                    CharRange([("5", "9")]),
                ]
            ),
        },
        {
            "description": "Bounded non-negative from zero (where min == 0, max > 0, and min != max): [0, 100]",
            "component": JSONInteger(minval=0, maxval=100),
            "expected": Or(
                [
                    String("100"),
                    And([CharRange([("1", "9")]), CharRange([("0", "9")])]),
                    CharRange([("1", "9")]),
                    String("0"),
                ],
            ),
        },
        {
            "description": "Bounded positive (where min > 0, max > 0, and min != max): [1, 100]",
            "component": JSONInteger(minval=1, maxval=100),
            "expected": Or(
                [
                    String("100"),
                    And([CharRange([("1", "9")]), CharRange([("0", "9")])]),
                    CharRange([("1", "9")]),
                ],
            ),
        },
    ],
)
def test_json_integer_grammar(test_case):
    actual = test_case["component"].grammar()
    assert actual == test_case["expected"], "\n".join(
        (
            f"Description: {test_case['description']!r}",
            f"Component: {fmt_result(test_case['component'])!s}",
            f"Expected: {fmt_result(test_case['expected'])!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_json_integer_invalid_range():
    with pytest.raises(ValueError, match="Invalid range"):
        JSONInteger(minval=10, maxval=5)


def test_json_integer_attrs_dict():
    comp = JSONInteger(minval=0, maxval=None)
    expected = {"minval": 0, "maxval": None}
    actual = comp.attrs_dict()
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(comp)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )


def test_json_integer_as_string():
    comp = JSONInteger(minval=0, maxval=None)
    expected = "JSONInteger(minval=0, maxval=None)"
    actual = comp.as_string(indent=None)
    assert actual == expected, "\n".join(
        (
            f"Component: {fmt_result(comp)!s}",
            f"Expected: {fmt_result(expected)!s}",
            f"Actual: {fmt_result(actual)!s}",
        )
    )
