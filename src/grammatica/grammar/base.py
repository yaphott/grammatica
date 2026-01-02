"""Base grammar classes for handling grammar expressions.

Provides abstractions for building and rendering grammar expressions.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import TYPE_CHECKING, cast

from grammatica.base import Component

if TYPE_CHECKING:
    from typing import Any


class Grammar(Component, ABC):
    """Base class for grammar expressions."""

    # __slots__: tuple[str, ...] = ()

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
        return cast(Grammar, super().copy())
