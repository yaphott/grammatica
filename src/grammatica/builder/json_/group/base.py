"""
Base grouped composition and rule builder classes that construct a grammar to match JSON object types.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComposition

if TYPE_CHECKING:
    from typing import Any

    from grammatica.grammar.base import Grammar


class GroupJSONComposition(JSONComposition, ABC):
    """Base class for grouped JSON compositions.

    Args:
        item_ws (Grammar | None): Whitespace grammar between items.
        key_ws (Grammar | None): Whitespace grammar between keys and values.
        n (int | tuple[int, int | None]): Minimum and maximum repetitions the grammar must match.

    Raises:
        ValueError: Range lower bound is negative.
        ValueError: Range upper bound is not positive or None (infinity).
        ValueError: Range lower bound is greater than range upper bound.
    """

    __slots__: tuple[str, ...] = ("item_ws", "key_ws", "n")

    def __init__(
        self,
        *,
        item_ws: Grammar | None,
        key_ws: Grammar | None,
        n: int | tuple[int, int | None],
    ) -> None:
        lower: int
        upper: int | None
        if isinstance(n, int):
            lower = upper = n
        else:  # tuple
            lower, upper = n
        if lower < 0:
            raise ValueError(f"n lower bound must be non-negative: {n}")
        if upper is not None:
            if upper < 0:
                raise ValueError(
                    f"n upper bound must be non-negative or None (infinity): {n}"
                )
            if lower > upper:
                raise ValueError(f"n lower bound must be <= n upper bound: {n}")
        self.n: tuple[int, int | None] = (lower, upper)
        """Minimum and maximum repetitions the grammar must match."""

        self.item_ws: Grammar | None = item_ws
        """Whitespace grammar between items."""
        self.key_ws: Grammar | None = key_ws
        """Whitespace grammar between keys and values."""

    @abstractmethod
    def attrs_dict(self) -> dict[str, Any]:
        return {
            "n": self.n,
            "item_ws": self.item_ws,
            "key_ws": self.key_ws,
        } | super().attrs_dict()

    @abstractmethod
    def grammar(self) -> Grammar: ...

    def _item_ws_subexprs(self) -> list[Grammar]:
        return [] if self.item_ws is None else [self.item_ws]

    def _key_ws_subexprs(self) -> list[Grammar]:
        return [] if self.key_ws is None else [self.key_ws]

    def _indent_subexprs(self) -> tuple[list[Grammar], list[Grammar]]:
        item_ws_subexprs = [] if self.item_ws is None else [self.item_ws]
        key_ws_subexprs = [] if self.key_ws is None else [self.key_ws]
        return item_ws_subexprs, key_ws_subexprs
