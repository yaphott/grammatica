"""
Utility functions.
"""

from __future__ import annotations

from collections.abc import Callable
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from collections.abc import Collection, Mapping
    from typing import Any

__all__ = [
    "char_to_cpoint",
    "char_to_hex",
    "ord_to_cpoint",
    "ord_to_hex",
    "value_to_string",
]


def char_to_cpoint(char: str) -> str:
    return ord_to_cpoint(ord(char))


def ord_to_cpoint(ordinal: int) -> str:
    if ordinal < 0x10000:
        return f"\\u{ordinal:04X}"
    return f"\\U{ordinal:08X}"


def char_to_hex(char: str) -> str:
    return ord_to_hex(ord(char))


def ord_to_hex(ordinal: int) -> str:
    return f"\\x{ordinal:02X}"


def _value_is_simple(value: Any) -> bool:
    """Determine if a value is simple (None, bool, int, float, or str).

    Args:
        value (Any): Value to check.

    Returns:
        bool: True if the value is simple, False otherwise.
    """
    if value is None:
        return True
    if isinstance(value, (int, float)):
        return True
    if isinstance(value, str):
        return True
    return False


def _collection_to_string(
    value: tuple[Any, ...] | list[Any] | set[Any] | frozenset[Any],
    indent: int | None,
) -> str:
    value_n = len(value)

    prefix: str
    suffix: str
    allow_trailing_comma: bool
    if isinstance(value, tuple):
        if value_n < 1:
            return "()"
        prefix, suffix = "(", ")"
        allow_trailing_comma = len(value) == 1
    elif isinstance(value, list):
        if value_n < 1:
            return "[]"
        prefix, suffix = "[", "]"
        allow_trailing_comma = False
    elif isinstance(value, set):
        if value_n < 1:
            return "set()"
        prefix, suffix = "{", "}"
        allow_trailing_comma = False
    else:  # frozenset
        if value_n < 1:
            return "frozenset()"
        prefix, suffix = "frozenset({", "})"
        allow_trailing_comma = False

    msg = prefix
    if (indent is None) or all(map(_value_is_simple, value)):
        # Render as a single line
        for i, subvalue in enumerate(value):
            if i > 0:
                msg += ", "
            msg += value_to_string(subvalue, indent=None)
            if allow_trailing_comma and (i >= value_n - 1):
                msg += ","
    else:
        # Render as multiple lines
        for i, subvalue in enumerate(value):
            if i > 0:
                msg += ","
            msg += "\n" + (" " * indent)
            msg += value_to_string(subvalue, indent=indent).replace(
                "\n",
                "\n" + (" " * indent),
            )
            if i >= value_n - 1:
                if allow_trailing_comma:
                    msg += ","
                msg += "\n"
    msg += suffix
    return msg


def _mapping_to_string(value: Mapping[Any, Any], indent: int | None) -> str:
    msg = "{"
    for j, k in enumerate(tuple(value)):
        subkey, subvalue = k, value[k]
        if indent is None:
            if j > 0:
                msg += ", "
        else:
            if j > 0:
                msg += ","
            msg += "\n" + (" " * indent)
        msg += value_to_string(subkey, indent=indent) + ": "
        if indent is None:
            msg += value_to_string(subvalue, indent=indent)
        else:
            msg += value_to_string(subvalue, indent=indent).replace(
                "\n",
                "\n" + (" " * indent),
            )
            if j == len(value) - 1:
                msg += "\n"
    msg += "}"
    return msg


def value_to_string(value: Any, indent: int | None) -> str:
    """Create a string representation of a value.

    Args:
        value (Any): Value to represent as a string.
        indent (int | None): Indentation level, or None for no indentation.

    Returns:
        str: String representation of the value.

    Raises:
        ValueError: Value type is unsupported.
    """
    if value is None:
        return "None"
    if isinstance(value, bool):
        return "True" if value else "False"
    if isinstance(value, (int, float)):
        return str(value)
    if isinstance(value, str):
        return repr(value)
    if isinstance(value, (tuple, list, set, frozenset)):
        return _collection_to_string(value, indent=indent)
    if isinstance(value, dict):
        return _mapping_to_string(value, indent=indent)
    if hasattr(value, "as_string") and isinstance(value.as_string, Callable):
        return value.as_string(indent=indent)
    raise ValueError(f"Unsupported value type: {type(value).__name__}")
