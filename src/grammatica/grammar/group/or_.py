"""
Classes and utilities for logical OR operations.
"""

from __future__ import annotations

import sys
from typing import TYPE_CHECKING, cast

from grammatica.grammar.group.and_ import And
from grammatica.grammar.group.base import GroupGrammar

if sys.version_info >= (3, 12):  # pragma: no cover
    from typing import override
else:  # pragma: no cover
    from typing_extensions import override

if TYPE_CHECKING:
    from collections.abc import Iterable

    from grammatica.grammar.base import Grammar


class Or(GroupGrammar):
    """Grouped grammar that represents a logical OR operation between grammars.

    Args:
        subexprs (Iterable[Grammar]): Grammars to group with OR logic.
        quantifier (int | tuple[int, int | None]): Minimum and maximum repetitions the expression must match. Defaults to (1, 1).

    Raises:
        ValueError: Range lower bound is negative.
        ValueError: Range upper bound is not positive or None (infinity).
        ValueError: Range lower bound is greater than range upper bound.

    Examples:
        >>> from grammatica.grammar import String
        >>> from grammatica.grammar.group import Or
        >>> g = Or([String("yes"), String("no")])
        >>> g
        Or(subexprs=[String(value='yes'), String(value='no')], quantifier=(1, 1))
    """

    separator: str = " | "
    """Separator to use for the grammar."""

    def __init__(
        self,
        subexprs: Iterable[Grammar],
        quantifier: int | tuple[int, int | None] = (1, 1),
    ) -> None:
        super().__init__(
            subexprs=subexprs,
            quantifier=quantifier,
        )

    def render(self, wrap: bool = True, **kwargs) -> str | None:
        """Render the grammar as a regular expression.

        Args:
            wrap (bool, optional): Wrap the expression in parentheses. Defaults to True.
            **kwargs: Keyword arguments for the current context.

        Returns:
            str | None: Rendered expression, or None if resolved to empty.

        Examples:
            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("yes"), String("no")])
            >>> print(g.render())
            ("yes" | "no")

            Optional (matching zero or one times) subexpressions render with a ``?`` quantifier

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("maybe"), String("perhaps")], quantifier=(0, 1))
            >>> print(g.render())
            ("maybe" | "perhaps")?

            Subexpressions matching n or more times (n > 1) render with a ``{n,}`` quantifier

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("ha"), String("ho")], quantifier=(3, None))
            >>> print(g.render())
            ("ha" | "ho"){3,}

            Subexpressions matching zero or more times render with a ``*`` quantifier

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("ha"), String("ho")], quantifier=(0, None))
            >>> print(g.render())
            ("ha" | "ho")*

            Subexpressions matching one or more times render with a ``+`` quantifier

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("ha"), String("ho")], quantifier=(1, None))
            >>> print(g.render())
            ("ha" | "ho")+

            Empty subexpressions render to :py:obj:`None`

            >>> from grammatica.grammar.group import Or
            >>> g = Or([])
            >>> print(g.render())
            None
        """
        return super().render(wrap=wrap, **kwargs)

    def needs_wrapped(self) -> bool:
        """Check if the expression needs to be wrapped in parentheses.

        Returns:
            bool: True if the expression needs to be wrapped, False otherwise.

        Examples:
            Empty subexpressions do not require wrapping

            >>> from grammatica.grammar.group import Or
            >>> g = Or([])
            >>> g.needs_wrapped()
            False

            Wrap when there are multiple subexpressions, no matter their quantifiers

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("yes"), String("no")])
            >>> g.needs_wrapped()
            True

            Do not wrap when there is a single subexpression that is not a grouped grammar (subclass of :class:`grammatica.grammar.group.GroupGrammar`)

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("samwise")])
            >>> g.needs_wrapped()
            False

            >>> from grammatica.grammar import CharRange
            >>> from grammatica.grammar.group import Or
            >>> g = Or(
            ...     [
            ...         CharRange([("a", "z")]),
            ...     ],
            ...     quantifier=(1, None),
            ... )
            >>> g.needs_wrapped()
            False

            Do not wrap when there is a single grouped subexpression that is a grouped grammar (subclass of :class:`grammatica.grammar.group.GroupGrammar`) and the parent has a default quantifier (default is :py:data:`(1, 1)`)

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import And, Or
            >>> two_digit_num = And(
            ...     [
            ...         CharRange([("1", "9")]),
            ...         CharRange([("0", "9")]),
            ...     ],
            ... )
            >>> g = Or([two_digit_num])
            >>> g.needs_wrapped()
            False

            Otherwise, iterate subexpressions recursively until one of the above conditions is met

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import And, Or
            >>> g = Or(
            ...     [
            ...         And(
            ...             [
            ...                 String("I'll be on the "),
            ...                 Or([String("red"), String("green"), String("blue")]),
            ...                 String(" team."),
            ...             ],
            ...         ),
            ...     ],
            ...     quantifier=(1, None),
            ... )
            >>> g.needs_wrapped()
            True
        """
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

    @override
    def simplify(self) -> Grammar | None:
        """Simplify the grammar.

        Attempts to reduce redundancy and optimize the grammar.
        Simplifying grouped grammars is a complex operation, and requires recursively employing multiple strategies.

        Note:
            The resulting grammar and its parts are copies, and the original grammar is not modified.

        Returns:
            Grammar | None: Simplified expression, or None if resolved to empty.

        See Also:
            :meth:`grammatica.grammar.group.Or.simplify_subexprs`: Low-level simplification of subexpressions for ``Or`` grouped grammars.

        Examples:
            All child subexpressions are simplified

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or(
            ...     [
            ...         And([String("a"), String("b")]),
            ...         And([String("c")]),
            ...     ],
            ... )
            >>> g.simplify()
            Or(subexprs=[String(value='ab'), String(value='c')], quantifier=(1, 1))

            Duplicate subexpressions are removed

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or(
            ...     [
            ...         String("frodo"),
            ...         String("sam"),
            ...         String("sam"),
            ...     ],
            ... )
            >>> g.simplify()
            Or(subexprs=[String(value='frodo'), String(value='sam')], quantifier=(1, 1))

            Empty subexpressions simplify to :py:obj:`None`

            >>> from grammatica.grammar.group import Or
            >>> g = Or([])
            >>> g.simplify() is None
            True

            Subexpressions that simplify to :py:obj:`None` are removed

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("hello"), String("")])
            >>> g.simplify()
            String(value='hello')

            Unwrap grouped grammars with a single subexpression and default quantifier (default is :py:data:`(1, 1)`)

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("only one")])
            >>> g.simplify()
            String(value='only one')

            Any grouped grammar with only a single subexpression is the same as an ``And`` grammar with a single subexpression

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or([String("only one")], quantifier=(0, 1))
            >>> g.simplify()
            And(subexprs=[String(value='only one')], quantifier=(0, 1))

            Grouped grammars with consecutive subexpressions that are grouped, have the same type, and have default
            quantifiers (default is :py:data:`(1, 1)`) are merged

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import Or
            >>> g = Or(
            ...     [
            ...         Or([String("a")]),
            ...         Or([String("b")]),
            ...         Or([String("c")]),
            ...     ],
            ... )
            >>> g.simplify()
            Or(subexprs=[String(value='a'), String(value='b'), String(value='c')], quantifier=(1, 1))

            When an ``Or`` grouped grammar that is optional (``(0, n)``) containing subexpressions that are all optional
            (``(0, n)``) does not need to then be optional itself

            >>> from grammatica.grammar import String
            >>> from grammatica.grammar.group import And, Or
            >>> g = Or(
            ...     [
            ...         And([String("frodo")], quantifier=(0, 1)),
            ...         And([String("sam")], quantifier=(0, 1)),
            ...     ],
            ...     quantifier=(0, 1),
            ... )
            >>> simplified = g.simplify()
            >>> print(simplified.as_string(indent=4))
            Or(
                subexprs=[
                    And(
                        subexprs=[
                            String(value='frodo')
                        ],
                        quantifier=(0, 1)
                    ),
                    And(
                        subexprs=[
                            String(value='sam')
                        ],
                        quantifier=(0, 1)
                    )
                ],
                quantifier=(1, 1)
            )

            Some advanced simplification strategies are not shown in the examples above.
        """
        return super().simplify()

    @staticmethod
    def simplify_subexprs(
        original_subexprs: list[Grammar],
        quantifier: tuple[int, int | None],
    ) -> Grammar | None:
        """Simplify the provided subexpressions for the grouped grammar.

        Note:
            The resulting grammar and its parts are copies, and the original grammar is not modified.

        Args:
            original_subexprs (list[Grammar]): Subexpressions to simplify.
            quantifier (tuple[int, int | None]): Quantifier for the expression.

        Returns:
            Grammar | None: Simplified expression. If the expression is empty, return None.

        See Also:
            :meth:`grammatica.grammar.group.Or.simplify`: High-level simplification of ``Or`` grouped grammars.
        """
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
