"""
Classes and utilities for defining derivation rules.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.grammar.base import BaseGrammar

if TYPE_CHECKING:
    from typing import Any


class DerivationRule(BaseGrammar):
    """Derivation rule that expands to a expression (grammar).

    Non-terminal symbol and an expression separated by a metasymbol (e.g. 'symbol ::= expression').
    """

    separator: str = " ::= "
    """Separator metasymbol to use for the derivation rule."""

    def __init__(self, symbol: str, value: BaseGrammar) -> None:
        """Derivation rule that expands to a grammar.

        Args:
            symbol (str): Symbol (non-terminal) for the derivation rule.
            value (BaseGrammar): Grammar the symbol derives into.
        """
        super().__init__()

        self.symbol: str = symbol
        """Symbol (non-terminal) for the derivation rule."""
        self.value: BaseGrammar = value
        """Grammar the symbol derives into."""

    def render(self, full: bool = True, wrap: bool = True) -> str | None:
        if not full:
            return self.symbol
        rendered = self.value.render(full=False, wrap=wrap)
        if not rendered:
            return None
        expr = f"{self.symbol}{self.separator}{rendered}"
        return expr

    def simplify(self) -> DerivationRule | None:
        simplified = self.value.simplify()
        if simplified is None:
            return None
        return DerivationRule(self.symbol, simplified)

    def attrs_dict(self) -> dict[str, Any]:
        return {"symbol": self.symbol, "value": self.value}
