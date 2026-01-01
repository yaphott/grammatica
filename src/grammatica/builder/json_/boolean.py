"""
Classes and utilities for building JSON boolean grammar components.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComponent
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from typing import Any

    from grammatica.grammar.base import Grammar


class JSONBoolean(JSONComponent):
    """Component that matches a JSON boolean value (``true`` or ``false``).

    See Also:
        :class:`grammatica.builder.json_.JSONBooleanLiteral`: JSON boolean component that matches a specific boolean value.

    Examples:
        Grammar that matches either JSON-encoded :py:obj:`True` or :py:obj:`False`

        >>> from grammatica.builder.json_ import JSONBoolean
        >>> component = JSONBoolean()
        >>> component
        JSONBoolean()
        >>> component.grammar()
        Or(items=[String(value='true'), String(value='false')])
    """
    def __init__(self) -> None:  # pylint: disable=W0246
        super().__init__()

    def attrs_dict(self) -> dict[str, Any]:  # pylint: disable=W0246
        return super().attrs_dict()

    def grammar(self) -> Or:
        """Construct a grammar for the component.

        Returns:
            Grammar: Grammar for the component.

        Examples:
            Grammar that matches either JSON-encoded value of :py:obj:`True` or :py:obj:`False`

            >>> from grammatica.builder.json_ import JSONBoolean
            >>> component = JSONBoolean()
            >>> g = component.grammar()
            >>> g
            Or(items=[String(value='true'), String(value='false')])
        """
        return Or([String("true"), String("false")])


class JSONBooleanLiteral(JSONComponent):
    """Component that matches a JSON boolean literal.

    Args:
        value (bool): Boolean value to match.

    See Also:
        :class:`grammatica.builder.json_.JSONBoolean`: JSON boolean component that matches either ``true`` or ``false``.

    Examples:
        Grammar that matches the JSON-encoded value of :py:obj:`False`

        >>> from grammatica.builder.json_ import JSONBooleanLiteral
        >>> component = JSONBooleanLiteral(False)
        >>> component
        JSONBooleanLiteral(value=False)
        >>> component.grammar()
        String(value='false')

        Grammar that matches the JSON-encoded value of :py:obj:`True`

        >>> from grammatica.builder.json_ import JSONBooleanLiteral
        >>> component = JSONBooleanLiteral(True)
        >>> component
        JSONBooleanLiteral(value=True)
        >>> component.grammar()
        String(value='true')
    """

    def __init__(self, value: bool) -> None:  # pylint: disable=W0246
        super().__init__()

        self.value: bool = value

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value} | super().attrs_dict()

    def grammar(self) -> Grammar:
        """Construct a grammar for the component.

        Returns:
            Grammar: Grammar for the component.

        Examples:
            Grammar that matches the JSON-encoded value of :py:obj:`False`

            >>> from grammatica.builder.json_ import JSONBooleanLiteral
            >>> component = JSONBooleanLiteral(False)
            >>> g = component.grammar()
            >>> g
            String(value='false')

            Grammar that matches the JSON-encoded value of :py:obj:`True`

            >>> from grammatica.builder.json_ import JSONBooleanLiteral
            >>> component = JSONBooleanLiteral(True)
            >>> g = component.grammar()
            >>> g
            String(value='true')
        """
        return String("true" if self.value else "false")
