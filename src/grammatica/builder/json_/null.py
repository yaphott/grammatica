"""
Classes and utilities for building JSON null literal grammar components.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComponent
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from typing import Any


class JSONNullLiteral(JSONComponent):
    """Component that matches a JSON null literal.

    Examples:
        >>> from grammatica.builder.json_ import JSONNullLiteral
        >>> component = JSONNullLiteral()
        >>> component
        JSONNullLiteral()
        >>> component.grammar()
        String(value='null')
    """

    def __init__(self) -> None:  # pylint: disable=W0246
        super().__init__()

    def attrs_dict(self) -> dict[str, Any]:  # pylint: disable=W0246
        return super().attrs_dict()

    def grammar(self) -> String:
        """Construct a grammar for the component.

        Returns:
            Grammar: Grammar for the component.

        Examples:
            Grammar that matches the JSON-encoded value of :py:obj:`None`

            >>> from grammatica.builder.json_ import JSONNullLiteral
            >>> component = JSONNullLiteral()
            >>> g = component.grammar()
            >>> g
            String(value='null')
        """
        return String("null")
