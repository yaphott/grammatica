"""Base grammar classes and utility functions for handling grouped grammar expressions.

Provides abstractions for building and rendering grouped grammar expressions, as well as utility
functions for grouping and merging subexpressions.
"""

from __future__ import annotations

import sys
from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

from grammatica.grammar.base import Grammar

if sys.version_info >= (3, 12):  # pragma: no cover
    from typing import override
else:  # pragma: no cover
    from typing_extensions import override


if TYPE_CHECKING:
    from collections.abc import Iterable
    from typing import Any


class GroupGrammar(Grammar, ABC):
    """Base class for grouped grammar expressions."""

    __slots__: tuple[str, ...] = ("separator", "subexprs", "quantifier")

    separator: str
    """Separator to use for the grammar."""

    def __init__(
        self,
        *,
        subexprs: Iterable[Grammar],
        quantifier: int | tuple[int, int | None],
    ) -> None:
        super().__init__()

        self.subexprs: list[Grammar] = list(subexprs)
        """Group of grammars."""

        lower: int
        upper: int | None
        if isinstance(quantifier, int):
            lower = upper = quantifier
        else:  # tuple
            lower, upper = quantifier
        if lower < 0:
            raise ValueError(f"Range lower bound must be non-negative: {quantifier}")
        if upper is not None:
            if upper < 1:
                raise ValueError(
                    f"Range upper bound must be positive or None (infinity): {quantifier}"
                )
            if lower > upper:
                raise ValueError(
                    f"Range lower bound must be <= range upper bound: {quantifier}"
                )
        self.quantifier: tuple[int, int | None] = (lower, upper)
        """Minimum and maximum repetitions the expression must match."""

    def render(self, full: bool = True, wrap: bool = True) -> str | None:
        if len(self.subexprs) < 1:
            return None
        rendered_quantifier = self.render_quantifier()
        expr = ""
        found = False
        for subexpr in self.subexprs:
            rendered = subexpr.render(full=False)
            if rendered is not None:
                if found:
                    expr += self.separator
                expr += rendered
                found = True
        if not found:
            return None
        if self.needs_wrapped() and (wrap or (rendered_quantifier is not None)):
            expr = "(" + expr + ")"
        if rendered_quantifier is not None:
            expr += rendered_quantifier
        return expr

    def render_quantifier(self) -> str | None:
        """Render the quantifier.

        Returns:
            str | None: A quantifier string or None if not applicable.
        """
        lower, upper = self.quantifier
        if lower == upper == 1:
            return None
        if lower == 0:
            if upper is None:
                return "*"
            if upper == 1:
                return "?"
            # NOTE: Not supported by llama.cpp
            # return "{," + str(upper) + "}"
        if upper is None:
            if lower == 1:
                return "+"
            return "{" + str(lower) + ",}"
        if lower == upper:
            return "{" + str(lower) + "}"
        return "{" + str(lower) + "," + str(upper) + "}"

    def simplify(self) -> Grammar | None:
        return self.simplify_subexprs(self.subexprs, self.quantifier)

    @staticmethod
    @abstractmethod
    def simplify_subexprs(
        original_subexprs: list[Grammar],
        quantifier: tuple[int, int | None],
    ) -> Grammar | None:
        """Simplify the provided subexpressions.

        Args:
            original_subexprs (list[Grammar]): Subexpressions to simplify.
            quantifier (tuple[int, int | None]): Quantifier for the expression.

        Returns:
            Grammar | None: Simplified expression. If the expression is empty, return None.
        """
        return None

    @abstractmethod
    def needs_wrapped(self) -> bool:
        """Check if the expression needs to be wrapped in parentheses.

        Returns:
            bool: True if the expression needs to be wrapped, False otherwise.
        """
        return False

    @override
    def attrs_dict(self) -> dict[str, Any]:
        return {
            "subexprs": self.subexprs,
            "quantifier": self.quantifier,
        } | super().attrs_dict()
