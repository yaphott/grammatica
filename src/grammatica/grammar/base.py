"""Base grammar classes for handling grammar expressions.

Provides abstractions for building and rendering grammar expressions.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

from grammatica.utils import value_to_string

if TYPE_CHECKING:
    from typing import Any


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
