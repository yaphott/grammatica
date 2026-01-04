"""
Classes and utilities for compositions that construct a grammar to match a JSON null.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComposition
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from typing import Any


class JSONNullLiteral(JSONComposition):
    """Composition that constructs a grammar to match a JSON null literal."""

    # __slots__: tuple[str, ...] = ()

    def __init__(self) -> None:  # pylint: disable=W0246
        super().__init__()

    def attrs_dict(self) -> dict[str, Any]:  # pylint: disable=W0246
        return super().attrs_dict()

    def grammar(self) -> String:
        """Construct a grammar for the composition.

        Returns:
            Grammar: Grammar for the composition.

        Examples:
            Create a composition and construct a grammar that matches a JSON ``null`` value

            >>> from grammatica.builder.json_ import JSONNullLiteral
            >>> comp = JSONNullLiteral()
            >>> g = comp.grammar()
            >>> g
            String(value='null')
        """
        return String("null")
