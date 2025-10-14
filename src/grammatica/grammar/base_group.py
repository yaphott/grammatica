"""Base grammar classes and utility functions for handling grouped grammar expressions.

Provides abstractions for building and rendering grouped grammar expressions, as well as utility
functions for grouping and merging subexpressions.
"""

from __future__ import annotations

import sys
from abc import ABC, abstractmethod
from collections import namedtuple
from typing import TYPE_CHECKING

import grammatica.grammar.grammar
from grammatica.grammar.base import BaseGrammar

if sys.version_info >= (3, 12):  # pragma: no cover
    from typing import override
else:  # pragma: no cover
    from typing_extensions import override


if TYPE_CHECKING:
    from collections.abc import Generator, Iterable
    from typing import Any


GroupWeight: type[tuple] = namedtuple(
    "GroupWeight",
    ["removed_size", "chunk_size", "count"],
)


class BaseGroupGrammar(BaseGrammar, ABC):
    separator: str
    """Separator to use for the grammar."""

    def __init__(
        self,
        *,
        subexprs: Iterable[BaseGrammar],
        quantifier: int | tuple[int, int | None],
    ) -> None:
        super().__init__()

        self.subexprs: list[BaseGrammar] = list(subexprs)
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

    def simplify(self) -> BaseGrammar | None:
        return self.simplify_subexprs(self.subexprs, self.quantifier)

    @staticmethod
    @abstractmethod
    def simplify_subexprs(
        original_subexprs: list[BaseGrammar],
        quantifier: tuple[int, int | None],
    ) -> BaseGrammar | None:
        """Simplify the provided subexpressions.

        Args:
            original_subexprs (list[BaseGrammar]): Subexpressions to simplify.
            quantifier (tuple[int, int | None]): Quantifier for the expression.

        Returns:
            BaseGrammar | None: Simplified expression. If the expression is empty, return None.
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


def _iter_group_spans(
    n: int,
    size: int,
    offset: int,
) -> Generator[tuple[int, int], None, None]:
    """Generate all possible index ranges that would produce slices of the provided size, starting at the provided offset.

    Args:
        n (int): Number of subexpressions.
        size (int): Size of the group.
        offset (int): Offset to start from.

    Yields:
        tuple[int, int]: Start and end indices of the group.
            The range is inclusive, meaning both start and end indices are included.
            For example, (0, 2) includes indices 0, 1, and 2.
    """
    for end in range(n - offset, size - 2, -size):
        start = end - size
        if start < 0:
            return
        yield start, end


def group_repeating_subexprs(
    subexprs: list[BaseGrammar],
    n: int,
) -> tuple[list[BaseGrammar], int]:
    """Group repeating sequences of subexpressions into a single subexpression.

    Todo:

    * Split up strings into smaller strings when performing grouping (`.explode()`).
    * Improve the features used to sort so that it takes the minimal number of grammars when counted recursively to the lowest non-group grammar types.
      This function would likely then call itself and/or simplify. Be sure to avoid grouping repeating characters in a string if lower than a given threshold.

    Args:
        subexprs (list[BaseGrammar]): Subexpressions to group.
        n (int): Length of the expression.

    Returns:
        tuple[list[BaseGrammar], int]: Grouped subexpressions and the number of subexpressions after grouping.
    """
    best_subexprs = []
    best_n = 0
    best_weight = GroupWeight(0, 0, 0)
    max_chunk_size = n // 2
    for chunk_size in range(max_chunk_size, 0, -1):
        max_offset_a = n - (chunk_size * 2)
        max_offset_b = chunk_size - (1 + (n % chunk_size))
        max_offset = max(max_offset_a, max_offset_b)
        for offset in range(max_offset + 1):
            count = 0
            cmp_slice = []
            start = 0  # Prevent unbounded warning
            for start, end in _iter_group_spans(n, chunk_size, offset):
                assert end - start == chunk_size
                # Record initial slice to compare against
                if count == 0:
                    cmp_slice = subexprs[start:end]
                    count += 1
                    continue

                # Record additional matching slice
                if cmp_slice == subexprs[start:end]:
                    count += 1
                    continue

                # Yield variation of subexpressions where repeating slices are grouped inside a grammar
                if count > 1:
                    new_n = n - (count * chunk_size) + 1
                    weight = GroupWeight(n - new_n, chunk_size, count)
                    if weight > best_weight:
                        grouped_grammar = grammatica.grammar.grammar.Grammar(
                            subexprs[start : start + chunk_size],
                            quantifier=(count, count),
                        )
                        best_subexprs = (
                            subexprs[:start]
                            + [grouped_grammar]
                            + subexprs[start + (count * chunk_size) :]
                        )
                        best_n = new_n
                        best_weight = weight

                # Reset comparison slice
                cmp_slice = subexprs[start:end]
                count = 1

            if count > 1:
                new_n = n - (count * chunk_size) + 1
                weight = GroupWeight(n - new_n, chunk_size, count)
                if weight > best_weight:
                    grouped_grammar = grammatica.grammar.grammar.Grammar(
                        subexprs[start : start + chunk_size],
                        quantifier=(count, count),
                    )
                    best_subexprs = (
                        subexprs[:start]
                        + [grouped_grammar]
                        + subexprs[start + (count * chunk_size) :]
                    )
                    best_n = new_n
                    best_weight = weight

    if best_weight > GroupWeight(0, 0, 0):
        assert len(best_subexprs) > 0
        assert best_n > 0
        return best_subexprs, best_n
    assert len(best_subexprs) == 0
    assert best_n == 0
    return subexprs, n
