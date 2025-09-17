"""
Classes and utilities for logical AND and OR operations.
"""

from __future__ import annotations

from collections import namedtuple
from typing import TYPE_CHECKING

from grammatica.grammar.base import BaseGrammar, BaseGroupGrammar
from grammatica.grammar.string import merge_adjacent_strings

if TYPE_CHECKING:
    from collections.abc import Generator, Iterable

GroupWeight: type[tuple] = namedtuple(
    "GroupWeight",
    ["removed_size", "chunk_size", "count"],
)


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
                        grouped_grammar = Grammar(
                            subexprs[start : start + chunk_size],
                            length_range=(count, count),
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
                    grouped_grammar = Grammar(
                        subexprs[start : start + chunk_size],
                        length_range=(count, count),
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


def merge_adjacent_default_or(subexprs: list[BaseGrammar], n: int) -> int:
    """Merge adjacent Or expressions having default quantifier (1, 1) in-place.

    Args:
        subexprs (list[BaseGrammar]): Subexpressions to merge.
        n (int): Number of subexpressions.

    Returns:
        int: Number of subexpressions after merging.
    """
    if n < 2:
        return n

    last_idx = -1
    for i in range(n - 1, -1, -1):
        if isinstance(subexprs[i], Or) and (subexprs[i].length_range == (1, 1)):
            if last_idx < 1:
                last_idx = i
            else:
                n -= 1
        else:
            if last_idx > 0:
                subexprs[i + 1] = Or(
                    [
                        s
                        for subexpr in subexprs[i + 1 : last_idx + 1]
                        for s in subexpr.subexprs
                    ]
                )
                del subexprs[i + 2 : last_idx + 1]
            last_idx = -1

    if last_idx > 0:
        subexprs[0] = Or(
            [s for subexpr in subexprs[0 : last_idx + 1] for s in subexpr.subexprs]
        )
        del subexprs[1 : last_idx + 1]

    return n


def merge_adjacent_default_grammar(subexprs: list[BaseGrammar], n: int) -> int:
    """Merge adjacent Grammar expressions having default quantifier (1, 1) in-place.

    Args:
        subexprs (list[BaseGrammar]): Subexpressions to merge.
        n (int): Number of subexpressions.

    Returns:
        int: Number of subexpressions after merging.
    """
    if n < 2:
        return n

    last_idx = -1
    for i in range(n - 1, -1, -1):
        if isinstance(subexprs[i], Grammar) and (subexprs[i].length_range == (1, 1)):
            if last_idx < 1:
                last_idx = i
            else:
                n -= 1
        else:
            if last_idx > 0:
                subexprs[i + 1] = Grammar(
                    [
                        s
                        for subexpr in subexprs[i + 1 : last_idx + 1]
                        for s in subexpr.subexprs
                    ]
                )
                del subexprs[i + 2 : last_idx + 1]
            last_idx = -1

    if last_idx > 0:
        subexprs[0] = Grammar(
            [s for subexpr in subexprs[0 : last_idx + 1] for s in subexpr.subexprs]
        )
        del subexprs[1 : last_idx + 1]

    return n


class Grammar(BaseGroupGrammar):
    """Logical AND operation between grammars."""

    separator: str = " "
    """Separator to use for the grammar."""

    def __init__(
        self,
        subexprs: Iterable[BaseGrammar],
        length_range: int | tuple[int, int | None] = (1, 1),
    ) -> None:
        """Grouped grammar that represents a logical AND operation between grammars.

        Args:
            subexprs (Iterable[BaseGrammar]): Grammars to group with AND logic.
            length_range (int | tuple[int, int | None]): Minimum and maximum repetitions the expression must match. Defaults to (1, 1).
        """
        super().__init__(
            subexprs=subexprs,
            length_range=length_range,
        )

    def needs_wrapped(self) -> bool:
        n = len(self.subexprs)
        if n < 1:
            return False
        if n == 1:
            if self.length_range == (1, 1):
                return False
            # Recursively look for the first non-default (1, 1) subexpression
            subexpr = self.subexprs[0]
            wrap = isinstance(subexpr, BaseGroupGrammar)
            while (
                wrap
                and (subexpr.length_range == (1, 1))
                and (len(subexpr.subexprs) == 1)
            ):
                subexpr = subexpr.subexprs[0]
                wrap = isinstance(subexpr, BaseGroupGrammar)
            return wrap
        return self.length_range != (1, 1)

    @staticmethod
    def simplify_subexprs(
        original_subexprs: list[BaseGrammar],
        length_range: tuple[int, int | None],
    ) -> BaseGrammar | None:
        subexprs: list[BaseGrammar] = []
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

        # Grammar that is (1, n) or optional (0, n) containing subexpressions that are all optional (0, n) and are equivalent to each other (excluding length range) can be simplified to a single subexpression with a quantifier
        # The upper bound of the length range is calculated by taking the sum of the upper bounds of the subexpressions and multiplying it by the upper bound of the outer expression
        if (
            length_range[0] in (0, 1)
            and all(
                map(
                    lambda x: isinstance(x, BaseGroupGrammar)
                    and x.length_range[0] == 0,
                    subexprs,
                )
            )
            and all(
                subexprs[i].equals(subexprs[i + 1], check_length_range=False)
                for i in range(n - 1)
            )
        ):
            upper_bound = (
                sum(map(lambda x: x.length_range[1], subexprs)) * length_range[1]
            )
            if isinstance(subexprs[0], BaseGroupGrammar):
                return subexprs[0].simplify_subexprs(
                    subexprs[0].subexprs, (0, upper_bound)
                )
            return Grammar.simplify_subexprs(
                [Grammar([subexprs[0]], length_range=(0, upper_bound))],
                length_range,
            )

        if n == 1:
            # Unwrap a single default (1, 1) subexpression
            if length_range == (1, 1):
                return subexprs[0]

            # Grammar that is optional (0, 1) can recursively unwrap to the first simple or non-single, non-default (1, 1), and non-optional (0, 1) grouped grammar.
            if (
                (length_range == (0, 1))
                and isinstance(subexprs[0], BaseGroupGrammar)
                and (subexprs[0].length_range in ((0, 1), (1, 1)))
            ):
                return Grammar.simplify_subexprs(subexprs[0].subexprs, length_range)

        merged_n = merge_adjacent_default_grammar(subexprs, n)
        if merged_n < n:
            return Grammar.simplify_subexprs(subexprs, length_range)

        merged_n = merge_adjacent_strings(subexprs, n)
        if merged_n < n:
            return Grammar.simplify_subexprs(subexprs, length_range)

        grouped, grouped_n = group_repeating_subexprs(subexprs, n)
        if grouped_n < n:
            return Grammar.simplify_subexprs(grouped, length_range)

        # Grammar that is optional (0, n) containing subexpressions that are all optional (0, n) does not need to then be optional itself
        if (length_range[0] == 0) and all(
            map(
                lambda x: isinstance(x, BaseGroupGrammar) and (x.length_range[0] == 0),
                subexprs,
            )
        ):
            return Grammar.simplify_subexprs(subexprs, (1, length_range[1]))

        return Grammar(subexprs, length_range=length_range)


class Or(BaseGroupGrammar):
    """Logical OR operation between grammars."""

    separator: str = " | "
    """Separator to use for the grammar."""

    def __init__(
        self,
        subexprs: Iterable[BaseGrammar],
        length_range: int | tuple[int, int | None] = (1, 1),
    ) -> None:
        """Grouped grammar that represents a logical OR operation between grammars.

        Args:
            subexprs (Iterable[BaseGrammar]): Grammars to group with OR logic.
            length_range (int | tuple[int, int | None]): Minimum and maximum repetitions the expression must match. Defaults to (1, 1).
        """
        super().__init__(
            subexprs=subexprs,
            length_range=length_range,
        )

    def needs_wrapped(self) -> bool:
        n = len(self.subexprs)
        if n < 1:
            return False
        if n == 1:
            if self.length_range == (1, 1):
                return False
            # Recursively look for the first non-default (1, 1) subexpression
            subexpr = self.subexprs[0]
            wrap = isinstance(subexpr, BaseGroupGrammar)
            while (
                wrap
                and (subexpr.length_range == (1, 1))
                and (len(subexpr.subexprs) == 1)
            ):
                subexpr = subexpr.subexprs[0]
                wrap = isinstance(subexpr, BaseGroupGrammar)
            return wrap
        return True

    @staticmethod
    def simplify_subexprs(
        original_subexprs: list[BaseGrammar],
        length_range: tuple[int, int | None],
    ) -> BaseGrammar | None:
        subexprs: list[BaseGrammar] = []
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
            if length_range == (1, 1):
                return subexprs[0]

            # Or that is optional (0, 1) can recursively unwrap to the first simple or non-single, non-default (1, 1), and non-optional (0, 1) grouped grammar.
            if (
                (length_range == (0, 1))
                and isinstance(subexprs[0], BaseGroupGrammar)
                and (subexprs[0].length_range in ((0, 1), (1, 1)))
            ):
                return Grammar.simplify_subexprs(subexprs[0].subexprs, length_range)

            # Or with a single subexpression is the same as Grammar with a single subexpression
            return Grammar.simplify_subexprs(subexprs, length_range)

        merged_n = merge_adjacent_default_or(subexprs, n)
        if merged_n < n:
            return Or.simplify_subexprs(subexprs, length_range)

        # Or that is optional (0, n) containing subexpressions that are all optional (0, n) does not need to then be optional itself
        if (length_range[0] == 0) and all(
            map(
                lambda x: isinstance(x, BaseGroupGrammar) and (x.length_range[0] == 0),
                subexprs,
            )
        ):
            return Or.simplify_subexprs(subexprs, (1, length_range[1]))

        return Or(subexprs, length_range=length_range)
