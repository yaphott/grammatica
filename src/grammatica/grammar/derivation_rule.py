"""
Classes and utilities for defining derivation rules.
"""

from __future__ import annotations

import sys
from typing import TYPE_CHECKING

from grammatica.grammar.base import Grammar, value_to_string

if sys.version_info >= (3, 12):  # pragma: no cover
    from typing import override
else:  # pragma: no cover
    from typing_extensions import override

if TYPE_CHECKING:
    from typing import Any


class DerivationRule(Grammar):
    """Derivation rule that expands to an expression (grammar).

    Non-terminal symbol and an expression separated by a metasymbol (e.g. 'symbol ::= expression').

    Args:
        symbol (str): Symbol (non-terminal) for the derivation rule.
        value (Grammar): Grammar the symbol derives into.

    Examples:
        >>> from grammatica.grammar import DerivationRule, CharRange
        >>> rule = DerivationRule(
        ...     "digits",
        ...     CharRange([("0", "9")]),
        ... )
        >>> print(rule.as_string(indent=4))
        DerivationRule(
            symbol='digits',
            value=CharRange(char_ranges=[('0', '9')], negate=False)
        )
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

    def render(self, full: bool = True, wrap: bool = True, **kwargs) -> str | None:
        """Render the grammar as a regular expression.

        Args:
            full (bool, optional): Render the full derivation rule. Defaults to True.
            wrap (bool, optional): Wrap the expression in parentheses. Defaults to True.
            **kwargs: Keyword arguments for the current context.

        Returns:
            str | None: Rendered expression, or None if resolved to empty.

        Examples:
            Create a derivation rule where the symbol ``digits`` derives into a :class:`grammatica.grammar.CharRange` grammar that matches digits ``0-9``:

            >>> from grammatica.grammar import DerivationRule, CharRange
            >>> rule = DerivationRule(
            ...     "digits",
            ...     CharRange([("0", "9")]),
            ... )
            >>> print(rule.render())
            digits ::= [0-9]

            Create a derivation rule where the symbol ``bool`` derives into an :class:`grammatica.grammar.group.Or` grammar that matches either ``true`` or ``false``:

            >>> from grammatica.grammar import DerivationRule, String
            >>> from grammatica.grammar.group import Or
            >>> rule = DerivationRule(
            ...     "bool",
            ...     Or([String("true"), String("false")]),
            ... )
            >>> print(rule.render())
            bool ::= ("true" | "false")
        """
        if not full:
            return self.symbol
        kwargs["full"] = False
        kwargs["wrap"] = wrap
        rendered = self.value.render(**kwargs)
        if not rendered:
            return None
        expr = f"{self.symbol}{self.separator}{rendered}"
        return expr

    def simplify(self) -> DerivationRule | None:
        """Simplify the grammar.

        Attempts to reduce redundancy, remove empty subexpressions, and optimize the grammar.

        Note:
            The resulting grammar and its parts are copies, and the original grammar is not modified.

        Returns:
            DerivationRule | None: Simplified expression, or None if resolved to empty.

        Examples:
            >>> from grammatica.grammar import CharRange, DerivationRule, String
            >>> from grammatica.grammar.group import And
            >>> hero = And([String("Samwise"), String(" "), String("Gamgee")])
            >>> rule = DerivationRule("hero", hero)
            >>> simplified_rule = rule.simplify()
            >>> simplified_rule
            DerivationRule(symbol='hero', value=String(value='Samwise Gamgee'))

            Expressions that simplify to :py:class:`None` result in the entire derivation rule simplifying to :py:class:`None`

            >>> from grammatica.grammar import String
            >>> empty_string = String("")
            >>> rule = DerivationRule("empty", empty_string)
            >>> simplified_rule = rule.simplify()
            >>> simplified_rule is None
            True
        """
        simplified = self.value.simplify()
        if simplified is None:
            return None
        return DerivationRule(self.symbol, simplified)

    def attrs_dict(self) -> dict[str, Any]:
        return {"symbol": self.symbol, "value": self.value}

    @override
    def as_string(self, indent: int | None = None, **kwargs) -> str:
        """Return a string representation of the grammar.

        Args:
            indent (int, optional): Number of spaces to indent each level. Defaults to None.
            **kwargs: Keyword arguments for the current context.

        Returns:
            str: String representation of the grammar.

        Raises:
            ValueError: Attribute type is not supported.
        """
        attrs = self.attrs_dict()
        n = len(attrs)
        kwargs["indent"] = indent
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
                msg += value_to_string(value, **kwargs)
            else:
                msg += value_to_string(value, **kwargs).replace(
                    "\n",
                    "\n" + (" " * indent),
                )
                if j == n - 1:
                    msg += "\n"
        msg += ")"
        return msg
