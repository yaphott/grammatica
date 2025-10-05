import pytest

from grammatica.utils import (
    char_to_cpoint,
    char_to_hex,
    ord_to_cpoint,
    ord_to_hex,
)


@pytest.mark.parametrize(
    "char,expected",
    [
        ("a", "\\u0061"),
        ("A", "\\u0041"),
        ("0", "\\u0030"),
        ("\n", "\\u000A"),
        ("€", "\\u20AC"),
        ("😀", "\\U0001F600"),  # Emoji (5+ char codepoint)
    ],
)
def test_char_to_cpoint(char, expected):
    assert char_to_cpoint(char) == expected


@pytest.mark.parametrize(
    "ordinal,expected",
    [
        (97, "\\u0061"),  # 'a'
        (65, "\\u0041"),  # 'A'
        (10, "\\u000A"),  # newline
        (8364, "\\u20AC"),  # '€'
        (128512, "\\U0001F600"),  # emoji (5+ digits)
    ],
)
def test_ord_to_cpoint(ordinal, expected):
    assert ord_to_cpoint(ordinal) == expected


@pytest.mark.parametrize(
    "char,expected",
    [
        ("a", "\\x61"),
        ("A", "\\x41"),
        ("0", "\\x30"),
        ("\n", "\\x0A"),
        ("~", "\\x7E"),
        ("😀", "\\x1F600"),
    ],
)
def test_char_to_hex(char, expected):
    assert char_to_hex(char) == expected


@pytest.mark.parametrize(
    "ordinal,expected",
    [
        (97, "\\x61"),  # 'a'
        (65, "\\x41"),  # 'A'
        (10, "\\x0A"),  # newline
        (126, "\\x7E"),  # '~'
        (255, "\\xFF"),  # max 2-digit hex
        (128512, "\\x1F600"),  # emoji (5+ digits)
    ],
)
def test_ord_to_hex(ordinal, expected):
    assert ord_to_hex(ordinal) == expected
