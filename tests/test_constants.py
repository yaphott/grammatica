import pytest

from grammatica.constants import (
    ALWAYS_SAFE_CHARS,
    CHAR_ESCAPE_MAP,
    RANGE_ESCAPE_CHARS,
    STRING_LITERAL_ESCAPE_CHARS,
)


@pytest.mark.parametrize(
    "char, expected",
    [
        # Digits
        ("0", True),
        ("5", True),
        ("9", True),
        # Lowercase letters
        ("a", True),
        ("m", True),
        ("z", True),
        # Uppercase letters
        ("A", True),
        ("M", True),
        ("Z", True),
        # Space
        (" ", True),
        # Punctuation
        ("!", True),
        ("#", True),
        ("$", True),
        ("%", True),
        ("&", True),
        ("'", True),
        ("(", True),
        (")", True),
        ("*", True),
        ("+", True),
        (",", True),
        ("-", True),
        (".", True),
        ("/", True),
        (":", True),
        (";", True),
        ("<", True),
        ("=", True),
        (">", True),
        ("?", True),
        ("@", True),
        ("[", True),
        ("]", True),
        ("^", True),
        ("_", True),
        ("`", True),
        ("{", True),
        ("|", True),
        ("}", True),
        ("~", True),
        # Not safe
        ("\n", False),
        ("\r", False),
        ("\t", False),
        ('"', False),
        ("\\", False),
        ("\x80", False),
        ("€", False),
        ("😀", False),
    ],
)
def test_always_safe_chars(char, expected):
    assert (char in ALWAYS_SAFE_CHARS) == expected


@pytest.mark.parametrize(
    "char, expected",
    [
        ("\n", "\\n"),
        ("\r", "\\r"),
        ("\t", "\\t"),
    ],
)
def test_char_escape_map_has_escape(char, expected):
    assert char in CHAR_ESCAPE_MAP
    assert CHAR_ESCAPE_MAP[char] == expected


@pytest.mark.parametrize(
    "char",
    [
        "a",
        " ",
        "0",
        '"',
        "\\",
        "^",
        "-",
        "[",
        "]",
    ],
)
def test_char_escape_map_no_escape(char):
    assert char not in CHAR_ESCAPE_MAP


@pytest.mark.parametrize(
    "char, expected",
    [
        ('"', True),
        ("\\", True),
        ("a", False),
        ("[", False),
        ("-", False),
        ("^", False),
        ("]", False),
        (" ", False),
    ],
)
def test_string_literal_escape_chars(char, expected):
    assert (char in STRING_LITERAL_ESCAPE_CHARS) == expected


@pytest.mark.parametrize(
    "char, expected",
    [
        ("^", True),
        ("-", True),
        ("[", True),
        ("]", True),
        ("\\", True),
        ("a", False),
        ('"', False),
        (" ", False),
    ],
)
def test_range_escape_chars(char, expected):
    assert (char in RANGE_ESCAPE_CHARS) == expected
