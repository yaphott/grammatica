"""
Classes and utilities for defining derivation rules.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.grammar.base import Grammar

if TYPE_CHECKING:
    from typing import Any


class DerivationRule(Grammar):
    """Derivation rule that expands to an expression (grammar).

    Non-terminal symbol and an expression separated by a metasymbol (e.g. 'symbol ::= expression').

    Args:
        symbol (str): Symbol (non-terminal) for the derivation rule.
        value (Grammar): Grammar the symbol derives into.

    Examples:
        Create a derivation rule where the symbol ``digits`` derives into a :class:`grammatica.grammar.CharRange` grammar that matches digits ``0-9``:

        >>> from grammatica.grammar import DerivationRule, CharRange
        >>> digits_grammar = CharRange([("0", "9")])
        >>> digits_rule = DerivationRule("digits", digits_grammar)
        >>> print(digits_rule.as_string(indent=4))
        DerivationRule(
            symbol='digits',
            value=CharRange(char_ranges=[('0', '9')], negate=False)
        )
        >>> print(digits_rule.render())
        digits ::= [0-9]

        Create a derivation rule where the symbol ``bool`` derives into an :class:`grammatica.grammar.group.Or` grammar that matches either ``true`` or ``false``:

        >>> from grammatica.grammar import DerivationRule, String
        >>> from grammatica.grammar.group import Or
        >>> bool_grammar = Or([String("true"), String("false")])
        >>> bool_rule = DerivationRule("bool", bool_grammar)
        >>> print(bool_rule.as_string(indent=4))
        DerivationRule(
            symbol='bool',
            value=Or(
                subexprs=[
                    String(value='true'),
                    String(value='false')
                ],
                quantifier=(1, 1)
            )
        )
        >>> print(bool_rule.render())
        bool ::= ("true" | "false")
    """

    __slots__: tuple[str, ...] = ("symbol", "value")

    separator: str = " ::= "
    """Separator metasymbol to use for the derivation rule."""

    def __init__(self, symbol: str, value: Grammar) -> None:
        super().__init__()

        self.symbol: str = symbol
        """Symbol (non-terminal) for the derivation rule."""
        self.value: Grammar = value
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
