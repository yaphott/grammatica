"""Base grammar classes and utility functions for handling grammar expressions.

Provides abstractions for building and rendering grammar expressions, as well as utility functions for
string conversion and determining the simplicity of values.
"""

from __future__ import annotations

import sys
from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

if sys.version_info >= (3, 12):  # pragma: no cover
    from typing import override
else:  # pragma: no cover
    from typing_extensions import override

if TYPE_CHECKING:
    from collections.abc import Iterable
    from typing import Any


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
        return None

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

    def as_string(self, indent: int | None = None) -> str:
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

    def equals(self, other: Any, check_quantifier: bool = True) -> bool:
        """Check equality with another value.

        Args:
            other (Any): Value to compare against.
            check_quantifier (bool, optional): Include the quantifier in the comparison. Defaults to True.

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
            if (k != "quantifier") or check_quantifier
        } == {
            k: other_attrs[k]
            for k in sorted(other_attrs)
            if (k != "quantifier") or check_quantifier
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
    separator: str
    """Separator to use for the grammar."""

    def __init__(
        self,
        *,
        subexprs: Iterable[BaseGrammar],
        quantifier: int | tuple[int, int | None],
    ) -> None:
        super().__init__()

        self.subexprs: list[BaseGrammar] = list(subexprs)
        """Group of grammars."""

        lower: int
        upper: int | None
        if isinstance(quantifier, int):
            lower = upper = quantifier
        else:  # tuple
            lower, upper = quantifier
        if lower < 0:
            raise ValueError(f"Range lower bound must be non-negative: {quantifier}")
        if upper is not None:
            if upper < 1:
                raise ValueError(
                    f"Range upper bound must be positive or None (infinity): {quantifier}"
                )
            if lower > upper:
                raise ValueError(
                    f"Range lower bound must be <= range upper bound: {quantifier}"
                )
        self.quantifier: tuple[int, int | None] = (lower, upper)
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
        """Render the quantifier.

        Returns:
            str | None: A quantifier string or None if not applicable.
        """
        lower, upper = self.quantifier
        if lower == upper == 1:
            return None
        if lower == 0:
            if upper is None:
                return "*"
            if upper == 1:
                return "?"
            # NOTE: Not supported by llama.cpp
            # return "{," + str(upper) + "}"
        if upper is None:
            if lower == 1:
                return "+"
            return "{" + str(lower) + ",}"
        if lower == upper:
            return "{" + str(lower) + "}"
        return "{" + str(lower) + "," + str(upper) + "}"

    def simplify(self) -> BaseGrammar | None:
        return self.simplify_subexprs(self.subexprs, self.quantifier)

    @staticmethod
    @abstractmethod
    def simplify_subexprs(
        original_subexprs: list[BaseGrammar],
        quantifier: tuple[int, int | None],
    ) -> BaseGrammar | None:
        """Simplify the provided subexpressions.

        Args:
            original_subexprs (list[BaseGrammar]): Subexpressions to simplify.
            quantifier (tuple[int, int | None]): Quantifier for the expression.

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
            "quantifier": self.quantifier,
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

    if isinstance(value, (tuple, list, set, frozenset)):
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

    raise ValueError(f"Unsupported value type: {type(value).__name__}")
