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
    """Composition that constructs a grammar that matches a JSON boolean value (``true`` or ``false``).

    See Also:
        :class:`grammatica.builder.json_.JSONBooleanLiteral`: JSON boolean composition that matches a specific value.

    Examples:
        Grammar that matches either JSON-encoded :py:obj:`True` or :py:obj:`False`

        >>> from grammatica.builder.json_ import JSONBoolean
        >>> comp = JSONBoolean()
        >>> comp
        JSONBoolean()
        >>> comp.grammar()
        Or(items=[String(value='true'), String(value='false')])
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
            Grammar that matches either JSON-encoded value of :py:obj:`True` or :py:obj:`False`

            >>> from grammatica.builder.json_ import JSONBoolean
            >>> comp = JSONBoolean()
            >>> g = comp.grammar()
            >>> g
            Or(items=[String(value='true'), String(value='false')])
        """
        return Or([String("true"), String("false")])


class JSONBooleanLiteral(JSONComposition):
    """Composition that constructs a grammar that matches a JSON boolean literal.

    Args:
        value (bool): Boolean value to match.

    See Also:
        :class:`grammatica.builder.json_.JSONBoolean`: JSON boolean composition that matches either ``true`` or ``false``.

    Examples:
        Component that constructs a grammar that matches the JSON value ``false``

        >>> from grammatica.builder.json_ import JSONBooleanLiteral
        >>> comp = JSONBooleanLiteral(False)
        >>> comp
        JSONBooleanLiteral(value=False)
        >>> comp.grammar()
        String(value='false')

        Component that constructs a grammar that matches the JSON value ``true``

        >>> from grammatica.builder.json_ import JSONBooleanLiteral
        >>> comp = JSONBooleanLiteral(True)
        >>> comp
        JSONBooleanLiteral(value=True)
        >>> comp.grammar()
        String(value='true')
    """

    __slots__: tuple[str, ...] = ("value",)

    def __init__(self, value: bool) -> None:  # pylint: disable=W0246
        super().__init__()

        self.value: bool = value

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value} | super().attrs_dict()

    def grammar(self) -> Grammar:
        """Construct a grammar for the composition.

        Returns:
            Grammar: Grammar for the composition.

        Examples:
            Component that constructs a grammar that matches the JSON value ``false``

            >>> from grammatica.builder.json_ import JSONBooleanLiteral
            >>> comp = JSONBooleanLiteral(False)
            >>> g = comp.grammar()
            >>> g
            String(value='false')

            Component that constructs a grammar that matches the JSON value ``true``

            >>> from grammatica.builder.json_ import JSONBooleanLiteral
            >>> comp = JSONBooleanLiteral(True)
            >>> g = comp.grammar()
            >>> g
            String(value='true')
        """
        return String("true" if self.value else "false")
