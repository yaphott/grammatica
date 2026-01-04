"""
Classes and utilities for compositions that construct a grammar to match a JSON boolean.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComposition
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from typing import Any

    from grammatica.grammar.base import Grammar


class JSONBoolean(JSONComposition):
    """Composition that constructs a grammar to match a JSON boolean value (``true`` or ``false``).

    See Also:
        :class:`grammatica.builder.json_.JSONBooleanLiteral`: JSON boolean composition that constructs a grammar to match a specific boolean value.
    """

    # __slots__: tuple[str, ...] = ()

    def __init__(self) -> None:  # pylint: disable=W0246
        super().__init__()

    def attrs_dict(self) -> dict[str, Any]:  # pylint: disable=W0246
        return super().attrs_dict()

    def grammar(self) -> Or:
        """Construct a grammar for the composition.

        Returns:
            Grammar: Grammar for the composition.

        Examples:
            Create a composition and construct a grammar that matches a JSON boolean value (``true`` or ``false``)

            >>> from grammatica.builder.json_ import JSONBoolean
            >>> comp = JSONBoolean()
            >>> g = comp.grammar()
            >>> g
            Or(items=[String(value='true'), String(value='false')])
        """
        return Or([String("true"), String("false")])


class JSONBooleanLiteral(JSONComposition):
    """Composition that constructs a grammar to match a JSON boolean literal.

    Args:
        value (bool): Boolean value to match.

    See Also:
        :class:`grammatica.builder.json_.JSONBoolean`: JSON boolean composition that constructs a grammar to match any boolean value.
    """

    __slots__: tuple[str, ...] = ("value",)

    def __init__(self, value: bool) -> None:  # pylint: disable=W0246
        super().__init__()

        self.value: bool = value
        """Boolean value to match."""

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value} | super().attrs_dict()

    def grammar(self) -> Grammar:
        """Construct a grammar for the composition.

        Returns:
            Grammar: Grammar for the composition.

        Examples:
            Create a composition and construct a grammar that matches the JSON boolean value ``false``

            >>> from grammatica.builder.json_ import JSONBooleanLiteral
            >>> comp = JSONBooleanLiteral(False)
            >>> g = comp.grammar()
            >>> g
            String(value='false')

            Create a composition and construct a grammar that matches the JSON boolean value ``true``

            >>> from grammatica.builder.json_ import JSONBooleanLiteral
            >>> comp = JSONBooleanLiteral(True)
            >>> g = comp.grammar()
            >>> g
            String(value='true')
        """
        return String("true" if self.value else "false")
