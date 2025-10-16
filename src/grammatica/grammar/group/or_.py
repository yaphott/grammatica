"""
Classes and utilities for logical OR operations.
"""

from __future__ import annotations

from typing import TYPE_CHECKING, cast

from grammatica.grammar.group.and_ import And
from grammatica.grammar.group.base import GroupGrammar

if TYPE_CHECKING:
    from collections.abc import Iterable

    from grammatica.grammar.base import Grammar


class Or(GroupGrammar):
    """Logical OR operation between grammars."""

    separator: str = " | "
    """Separator to use for the grammar."""

    def __init__(
        self,
        subexprs: Iterable[Grammar],
        quantifier: int | tuple[int, int | None] = (1, 1),
    ) -> None:
        """Grouped grammar that represents a logical OR operation between grammars.

        Args:
            subexprs (Iterable[Grammar]): Grammars to group with OR logic.
            quantifier (int | tuple[int, int | None]): Minimum and maximum repetitions the expression must match. Defaults to (1, 1).
        """
        super().__init__(
            subexprs=subexprs,
            quantifier=quantifier,
        )

    def needs_wrapped(self) -> bool:
        n = len(self.subexprs)
        if n < 1:
            return False
        if n == 1:
            if self.quantifier == (1, 1):
                return False
            # Recursively look for the first non-default (1, 1) subexpression
            subexpr = self.subexprs[0]
            wrap = isinstance(subexpr, GroupGrammar)
            while (
                wrap
                and (cast(GroupGrammar, subexpr).quantifier == (1, 1))
                and (len(cast(GroupGrammar, subexpr).subexprs) == 1)
            ):
                subexpr = cast(GroupGrammar, subexpr).subexprs[0]
                wrap = isinstance(subexpr, GroupGrammar)
            return wrap
        return True

    @staticmethod
    def simplify_subexprs(
        original_subexprs: list[Grammar],
        quantifier: tuple[int, int | None],
    ) -> Grammar | None:
        subexprs: list[Grammar] = []
        n = 0
        for subexpr in original_subexprs:
            simplified = subexpr.simplify()
            if simplified is None:
                continue
            if simplified in subexprs:
                continue
            subexprs.append(simplified)
            n += 1

        # Empty expression is no-op
        if n < 1:
            return None

        if n == 1:
            # Unwrap a single default (1, 1) subexpression
            if quantifier == (1, 1):
                return subexprs[0]
            # Or that is optional (0, 1) can recursively unwrap to the first simple or non-single, non-default (1, 1), and non-optional (0, 1) grouped grammar.
            if (
                (quantifier == (0, 1))
                and isinstance(subexprs[0], GroupGrammar)
                and (subexprs[0].quantifier in ((0, 1), (1, 1)))
            ):
                return And.simplify_subexprs(subexprs[0].subexprs, quantifier)
            # Or with a single subexpression is the same as And with a single subexpression
            return And.simplify_subexprs(subexprs, quantifier)

        merged_n = merge_adjacent_default_or_grammars(subexprs, n)
        if merged_n < n:
            return Or.simplify_subexprs(subexprs, quantifier)

        # Or that is optional (0, n) containing subexpressions that are all optional (0, n) does not need to then be optional itself
        if (quantifier[0] == 0) and all(
            map(
                lambda x: isinstance(x, GroupGrammar) and (x.quantifier[0] == 0),
                subexprs,
            )
        ):
            return Or.simplify_subexprs(subexprs, (1, quantifier[1]))

        return Or(subexprs, quantifier=quantifier)


def merge_adjacent_default_or_grammars(subexprs: list[Grammar], n: int) -> int:
    """Merge adjacent Or expressions having default quantifier (1, 1) in-place.

    Args:
        subexprs (list[Grammar]): Subexpressions to merge.
        n (int): Number of subexpressions.

    Returns:
        int: Number of subexpressions after merging.
    """
    if n < 2:
        return n

    last_idx = -1
    for i in range(n - 1, -1, -1):
        if isinstance(subexprs[i], Or) and (cast(Or, subexprs[i]).quantifier == (1, 1)):
            if last_idx < 1:
                last_idx = i
            else:
                n -= 1
        else:
            if last_idx > 0:
                subexprs[i + 1] = Or(
                    (
                        s
                        for subexpr in subexprs[i + 1 : last_idx + 1]
                        for s in cast(Or, subexpr).subexprs
                    )
                )
                del subexprs[i + 2 : last_idx + 1]
            last_idx = -1

    if last_idx > 0:
        subexprs[0] = Or(
            (
                s
                for subexpr in subexprs[0 : last_idx + 1]
                for s in cast(Or, subexpr).subexprs
            )
        )
        del subexprs[1 : last_idx + 1]

    return n
