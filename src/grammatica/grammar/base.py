"""Base grammar classes and utility functions for handling grammar expressions.

Provides abstractions for building and rendering grammar expressions, as well as utility
functions for string conversion and determining the simplicity of values.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import (
        Any,
        Collection,
        Mapping,
    )


class Grammar(ABC):
    """Base class for grammar expressions."""

    __slots__: tuple[str, ...] = ()

    @abstractmethod
    def render(self, **kwargs) -> str | None:
        """Render the grammar as a regular expression.

        Args:
            **kwargs: Keyword arguments for the current context.

        Returns:
            str | None: Rendered expression, or None if resolved to empty.
        """
        return None

    @abstractmethod
    def simplify(self) -> Grammar | None:
        """Simplify the grammar.

        Attempts to reduce redundancy and optimize the grammar.

        Note:
            The resulting grammar and its parts are copies, and the original grammar is not modified.

        Returns:
            Grammar | None: Simplified expression, or None if resolved to empty.
        """
        return None

    @abstractmethod
    def attrs_dict(self) -> dict[str, Any]:
        """Return instance attributes of the grammar.

        Returns:
            dict[str, Any]: Dictionary of instance attributes.
        """
        return {}

    def copy(self) -> Grammar:
        """Create a copy of the grammar.

        Returns:
            Grammar: Copy of the grammar.
        """
        cls = type(self)
        kwargs = self.attrs_dict()
        g = cls(**kwargs)
        return g

    def as_string(self, **kwargs) -> str:
        """Return a string representation of the grammar.

        Args:
            **kwargs: Keyword arguments for the current context.

        Returns:
            str: String representation of the grammar.

        Raises:
            ValueError: Attribute type is not supported.
        """
        attrs = self.attrs_dict()
        kwargs["indent"] = None
        msg = f"{type(self).__name__}("
        for j, (name, value) in enumerate(attrs.items()):
            if j > 0:
                msg += ", "
            msg += f"{name}="
            msg += value_to_string(value, **kwargs)
        msg += ")"
        return msg

    def __repr__(self) -> str:
        return self.__str__()

    def __str__(self) -> str:
        return self.as_string(indent=None)

    def equals(self, other: Any, **kwargs) -> bool:
        """Check equality with another value.

        Args:
            other (Any): Value to compare against.
            **kwargs: Keyword arguments for the current context.

        Returns:
            bool: True if the values are equal, False otherwise.
        """
        if self is other:
            return True
        if not isinstance(other, type(self)):
            return False
        attrs = self.attrs_dict()
        other_attrs = other.attrs_dict()
        return {k: attrs[k] for k in sorted(attrs)} == {
            k: other_attrs[k] for k in sorted(other_attrs)
        }

    def __eq__(self, other: Any) -> bool:
        return self.equals(other)


def value_is_simple(value: Any) -> bool:
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


def _collection_to_string(value: Collection[Any], indent: int | None) -> str:
    prefix: str
    suffix: str
    if isinstance(value, tuple):
        prefix, suffix = "(", ")"
    elif isinstance(value, list):
        prefix, suffix = "[", "]"
    elif isinstance(value, set):
        if not value:
            return "set()"
        prefix, suffix = "{", "}"
    else:  # frozenset
        if not value:
            return "frozenset()"
        prefix, suffix = "frozenset({", "})"
    msg = prefix
    if all(map(value_is_simple, value)):
        for j, subvalue in enumerate(value):
            if j > 0:
                msg += ", "
            msg += value_to_string(subvalue, indent=None)
    else:
        value_n = len(value)
        for j, subvalue in enumerate(value):
            if indent is None:
                if j > 0:
                    msg += ", "
                msg += value_to_string(subvalue, indent=indent)
            else:
                if j > 0:
                    msg += ","
                msg += "\n" + (" " * indent)
                msg += value_to_string(subvalue, indent=indent).replace(
                    "\n",
                    "\n" + (" " * indent),
                )
                if j == value_n - 1:
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
    if isinstance(value, Grammar):
        return value.as_string(indent=indent)
    raise ValueError(f"Unsupported value type: {type(value).__name__}")
