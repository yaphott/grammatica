"""
Classes and utilities for logical AND operations.
"""

from __future__ import annotations

from collections import namedtuple
from typing import TYPE_CHECKING, cast

from grammatica.grammar.group.base import GroupGrammar
from grammatica.grammar.string import merge_adjacent_string_grammars

if TYPE_CHECKING:
    from collections.abc import Generator, Iterable

    from grammatica.grammar.base import Grammar


GroupWeight: type[tuple] = namedtuple(
    "GroupWeight",
    ["removed_size", "chunk_size", "count"],
)


class And(GroupGrammar):
    """Logical AND operation between grammars."""

    separator: str = " "
    """Separator to use for the grammar."""

    def __init__(
        self,
        subexprs: Iterable[Grammar],
        quantifier: int | tuple[int, int | None] = (1, 1),
    ) -> None:
        """Grouped grammar that represents a logical AND operation between grammars.

        Args:
            subexprs (Iterable[Grammar]): Grammars to group with AND logic.
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
        return self.quantifier != (1, 1)

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
            subexprs.append(simplified)
            n += 1

        # Empty expression is no-op
        if n < 1:
            return None

        # And that is (1, n) or optional (0, n) containing subexpressions that are all optional (0, n) and are equivalent to each other (excluding quantifier) can be simplified to a single subexpression with a quantifier
        # The upper bound of the quantifier is calculated by taking the sum of the upper bounds of the subexpressions and multiplying it by the upper bound of the outer expression
        if (
            quantifier[0] in (0, 1)
            and all(
                map(
                    lambda x: isinstance(x, GroupGrammar) and x.quantifier[0] == 0,
                    subexprs,
                )
            )
            and all(
                subexprs[i].equals(subexprs[i + 1], check_quantifier=False)
                for i in range(n - 1)
            )
        ):
            upper_bound = sum(
                map(
                    lambda x: cast(int, cast(GroupGrammar, x).quantifier[1]),
                    subexprs,
                )
            ) * cast(int, quantifier[1])
            return cast(GroupGrammar, subexprs[0]).simplify_subexprs(
                cast(GroupGrammar, subexprs[0]).subexprs,
                (0, upper_bound),
            )

        if n == 1:
            # Unwrap a single default (1, 1) subexpression
            if quantifier == (1, 1):
                return subexprs[0]
            # And that is optional (0, 1) can recursively unwrap to the first simple or non-single, non-default (1, 1), and non-optional (0, 1) grouped grammar.
            if (
                (quantifier == (0, 1))
                and isinstance(subexprs[0], GroupGrammar)
                and (subexprs[0].quantifier in ((0, 1), (1, 1)))
            ):
                return And.simplify_subexprs(subexprs[0].subexprs, quantifier)

        merged_n = merge_adjacent_default_and_grammars(subexprs, n)
        if merged_n < n:
            return And.simplify_subexprs(subexprs, quantifier)

        merged_n = merge_adjacent_string_grammars(subexprs, n)
        if merged_n < n:
            return And.simplify_subexprs(subexprs, quantifier)

        grouped, grouped_n = group_repeating_subexprs(subexprs, n)
        if grouped_n < n:
            return And.simplify_subexprs(grouped, quantifier)

        # And that is optional (0, n) containing subexpressions that are all optional (0, n) does not need to then be optional itself
        if (quantifier[0] == 0) and all(
            map(
                lambda x: isinstance(x, GroupGrammar) and (x.quantifier[0] == 0),
                subexprs,
            )
        ):
            return And.simplify_subexprs(subexprs, (1, quantifier[1]))

        return And(subexprs, quantifier=quantifier)


def merge_adjacent_default_and_grammars(subexprs: list[Grammar], n: int) -> int:
    """Merge adjacent And expressions having default quantifier (1, 1) in-place.

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
        if isinstance(subexprs[i], And) and (
            cast(And, subexprs[i]).quantifier == (1, 1)
        ):
            if last_idx < 1:
                last_idx = i
            else:
                n -= 1
        else:
            if last_idx > 0:
                subexprs[i + 1] = And(
                    (
                        s
                        for subexpr in subexprs[i + 1 : last_idx + 1]
                        for s in cast(And, subexpr).subexprs
                    )
                )
                del subexprs[i + 2 : last_idx + 1]
            last_idx = -1

    if last_idx > 0:
        subexprs[0] = And(
            (
                s
                for subexpr in subexprs[0 : last_idx + 1]
                for s in cast(And, subexpr).subexprs
            )
        )
        del subexprs[1 : last_idx + 1]

    return n


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
    subexprs: list[Grammar],
    n: int,
) -> tuple[list[Grammar], int]:
    """Group repeating sequences of subexpressions into a single subexpression.

    Todo:

    * Split up strings into smaller strings when performing grouping (`.explode()`).
    * Improve the features used to sort so that it takes the minimal number of grammars when counted recursively to the lowest non-group grammar types.
      This function would likely then call itself and/or simplify. Be sure to avoid grouping repeating characters in a string if lower than a given threshold.

    Args:
        subexprs (list[Grammar]): Subexpressions to group.
        n (int): Length of the expression.

    Returns:
        tuple[list[Grammar], int]: Grouped subexpressions and the number of subexpressions after grouping.
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
                        grouped_grammar = And(
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
                    grouped_grammar = And(
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
