"""Base grammar classes and utility functions for handling grammar expressions.

Provides abstractions for building and rendering grammar expressions, as well as utility functions for
string conversion and determining the simplicity of values.
"""

from __future__ import annotations

from abc import ABC, abstractmethod

try:
    from typing import override
except ImportError:
    from typing_extensions import override
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from collections.abc import Iterable
    from typing import Any

    from grammatica.grammar.base import BaseGrammar


class BaseGrammar(ABC):
    """Represents a base grammar expression.

    Provides an abstraction for rendering, simplifying, and copying grammar expressions.
    """

    @abstractmethod
    def render(self, full: bool = True, wrap: bool = True) -> str | None:
        """Render the grammar as a regular expression.

        Args:
            full (bool, optional): Render the full expression. Defaults to True.
            wrap (bool, optional): Wrap the expression in parentheses. Defaults to True.

        Returns:
            str | None: Rendered expression, or None if resolved to empty.
        """

    @abstractmethod
    def simplify(self) -> BaseGrammar | None:
        """Simplify the grammar.

        Attempts to reduce redundancy, remove empty subexpressions, and optimize the grammar.
        The resulting grammar and its parts are copies, and the original grammar is not modified.

        Returns:
            BaseGrammar | None: Simplified expression, or None if resolved to empty.
        """
        return None

    @abstractmethod
    def attrs_dict(self) -> dict[str, Any]:
        """Return instance attributes of the grammar.

        Returns:
            dict[str, Any]: Dictionary of instance attributes.
        """
        return {}

    def as_string(
        self,
        indent: int | None = None,
    ) -> str:
        """Return a string representation of the grammar.

        Args:
            indent (int, optional): Number of spaces to indent each level. Defaults to None.

        Returns:
            str: String representation of the grammar.

        Raises:
            ValueError: Attribute type is not supported.
        """
        attrs = self.attrs_dict()
        n = len(attrs)
        msg = f"{type(self).__name__}("
        for j, (name, value) in enumerate(attrs.items()):
            if indent is None:
                if j > 0:
                    msg += ", "
            else:
                if j > 0:
                    msg += ","
                msg += "\n" + (" " * indent)
            msg += f"{name}="
            if indent is None:
                msg += value_to_string(value, indent=indent)
            else:
                msg += value_to_string(value, indent=indent).replace(
                    "\n",
                    "\n" + (" " * indent),
                )
                if j == n - 1:
                    msg += "\n"
        msg += ")"
        return msg

    def __repr__(self) -> str:
        return self.__str__()

    def __str__(self) -> str:
        return self.as_string(indent=None)

    def equals(self, other: Any, check_length_range: bool = True) -> bool:
        """Check equality with another value.

        Args:
            other (Any): Value to compare against.
            check_length_range (bool, optional): Include the length range in the comparison. Defaults to True.

        Returns:
            bool: True if the values are equal, False otherwise.
        """
        if self is other:
            return True
        if not isinstance(other, BaseGrammar):
            return False
        attrs = self.attrs_dict()
        other_attrs = other.attrs_dict()
        return {
            k: attrs[k]
            for k in sorted(attrs)
            if (k != "length_range") or check_length_range
        } == {
            k: other_attrs[k]
            for k in sorted(other_attrs)
            if (k != "length_range") or check_length_range
        }

    def __eq__(self, other: Any) -> bool:
        return self.equals(other)

    def copy(self) -> BaseGrammar:
        """Create a copy of the grammar.

        Returns:
            BaseGrammar: Copy of the grammar.
        """
        cls = type(self)
        kwargs = self.attrs_dict()
        g = cls(**kwargs)
        return g


class BaseGroupGrammar(BaseGrammar, ABC):
    def __init__(
        self,
        *,
        subexprs: Iterable[BaseGrammar],
        length_range: int | tuple[int, int | None],
    ) -> None:
        super().__init__()

        self.subexprs: list[BaseGrammar] = list(subexprs)
        """Group of grammars."""

        if isinstance(length_range, int):
            lower = upper = length_range
        else:
            lower, upper = length_range
        if lower < 0:
            raise ValueError(f"Range start must be non-negative: {length_range}")
        if upper is not None:
            if upper < 1:
                raise ValueError(
                    f"Range end must be positive or None (infinity): {length_range}"
                )
            if lower > upper:
                raise ValueError(f"Range start must be <= range end: {length_range}")
        else:
            if lower < 0:
                raise ValueError(
                    f"Range start must be non-negative or None if end is None: {length_range}"
                )
        self.length_range: tuple[int, int | None] = (lower, upper)
        """Minimum and maximum repetitions the expression must match."""

    def render(self, full: bool = True, wrap: bool = True) -> str | None:
        if len(self.subexprs) < 1:
            return None
        rendered_quantifier = self.render_quantifier()
        expr = ""
        found = False
        for subexpr in self.subexprs:
            rendered = subexpr.render(full=False)
            if rendered is not None:
                if found:
                    expr += self.separator
                expr += rendered
                found = True
        if not found:
            return None
        if self.needs_wrapped() and (wrap or (rendered_quantifier is not None)):
            expr = "(" + expr + ")"
        if rendered_quantifier is not None:
            expr += rendered_quantifier
        return expr

    def render_quantifier(self) -> str | None:
        """Render the quantifier based on the length range.

        Returns:
            str | None: A quantifier string or None if not applicable.
        """
        lower, upper = self.length_range
        if lower == upper == 1:
            return None
        if lower == 0:
            if upper is None:
                return "*"
            if upper == 1:
                return "?"
            return "{," + str(upper) + "}"
        if upper is None:
            if lower == 1:
                return "+"
            return "{" + str(lower) + ",}"
        if lower == upper:
            length = lower
            if length == 1:
                return ""
            return "{" + str(length) + "}"
        return "{" + str(lower) + "," + str(upper) + "}"

    def simplify(self) -> BaseGrammar | None:
        return self.simplify_subexprs(self.subexprs, self.length_range)

    @staticmethod
    @abstractmethod
    def simplify_subexprs(
        original_subexprs: list[BaseGrammar],
        length_range: tuple[int, int | None],
    ) -> BaseGrammar | None:
        """Simplify the provided subexpressions.

        Args:
            original_subexprs (list[BaseGrammar]): Subexpressions to simplify.
            length_range (tuple[int, int | None]): Length range of the expression.

        Returns:
            BaseGrammar | None: Simplified expression. If the expression is empty, return None.
        """
        return None

    @abstractmethod
    def needs_wrapped(self) -> bool:
        """Check if the expression needs to be wrapped in parentheses.

        Returns:
            bool: True if the expression needs to be wrapped, False otherwise.
        """
        return False

    @override
    def attrs_dict(self) -> dict[str, Any]:
        return {
            "subexprs": self.subexprs,
            "length_range": self.length_range,
        } | super().attrs_dict()


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

    if isinstance(value, BaseGrammar):
        return value.as_string(indent=indent)

    if isinstance(value, (tuple, list, set)):
        if isinstance(value, tuple):
            braces = "(", ")"
        elif isinstance(value, list):
            braces = "[", "]"
        else:  # set
            braces = "{", "}"
        msg = braces[0]
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
        msg += braces[1]
        return msg

    if isinstance(value, dict):
        msg = "{"
        for j, (subkey, subvalue) in enumerate(value.items()):
            if indent is None:
                if j > 0:
                    msg += ", "
            else:
                if j > 0:
                    msg += ","
                msg += "\n" + (" " * indent)
            msg += f"{subkey}: "
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

    raise ValueError(f"Unsupported value type: {type(value)}")
