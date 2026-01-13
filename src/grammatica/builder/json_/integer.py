"""
Classes and utilities for compositions that construct a grammar to match a JSON integer.
"""

from __future__ import annotations

from typing import TYPE_CHECKING, cast

from grammatica.builder.json_.base import JSONComposition
from grammatica.grammar.base import Grammar
from grammatica.grammar.char_range import CharRange
from grammatica.grammar.group.and_ import And
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from typing import Any


class JSONInteger(JSONComposition):
    """Composition that constructs a grammar to match a JSON integer.

    Args:
        minval (int | None): Minimum value to match (inclusive), or None if unbounded.
        maxval (int | None): Maximum value to match (inclusive), or None if unbounded.

    Raises:
        ValueError: Minimum value is greater than maximum value.
    """

    __slots__: tuple[str, ...] = ("minval", "maxval")

    def __init__(
        self,
        minval: int | None = None,
        maxval: int | None = None,
    ) -> None:
        super().__init__()

        if (minval is not None) and (maxval is not None) and (minval > maxval):
            raise ValueError(f"Invalid range: {minval!r} > {maxval!r}")
        self.minval: int | None = minval
        """Minimum value to match (inclusive), or None if unbounded."""
        self.maxval: int | None = maxval
        """Maximum value to match (inclusive), or None if unbounded."""

    def attrs_dict(self) -> dict[str, Any]:
        return {
            "minval": self.minval,
            "maxval": self.maxval,
        } | super().attrs_dict()

    def grammar(self) -> Grammar:
        """Construct a grammar for the composition.

        Returns:
            Grammar: Grammar for the composition.

        Examples:
            Create a composition and construct a grammar that matches a JSON integer value ``-5 <= x <= 5``

            >>> from grammatica.builder.json_ import JSONInteger
            >>> comp = JSONInteger(minval=-5, maxval=5)
            >>> g = comp.grammar()
            >>> print(g.as_string(indent=4))
            Or(
                subexprs=[
                    And(
                        subexprs=[
                            String(value='-'),
                            CharRange(char_ranges=[('1', '5')], negate=False)
                        ],
                        quantifier=(1, 1)
                    ),
                    CharRange(char_ranges=[('1', '5')], negate=False),
                    String(value='0')
                ],
                quantifier=(1, 1)
            )
        """
        if (self.minval is None) and (self.maxval is None):
            # Boundless:
            #   (-infinity, infinity)
            return Or(
                [
                    # Non-zero: (-infinity, -1] U [1, infinity)
                    And(
                        [
                            And([String("-")], quantifier=(0, 1)),
                            CharRange([("1", "9")]),
                            And(
                                [CharRange([("0", "9")])],
                                quantifier=(0, None),
                            ),
                        ],
                    ),
                    # Zero: [0, 0]
                    String("0"),
                ]
            )
        elif self.maxval is None:
            self.minval = cast(int, self.minval)
            if self.minval < 0:
                # Lower bound is negative and no upper bound:
                #   [minval, infinity) where minval < 0
                abs_lo = abs(self.minval)
                return Or(
                    [
                        # Negative: (-infinity, minval] where minval < 0
                        And([String("-"), self._unbounded_above_expr(abs_lo)]),
                        # Zero: [0, 0]
                        String("0"),
                        # Positive: [1, infinity)
                        And(
                            [
                                CharRange([("1", "9")]),
                                And(
                                    [CharRange([("0", "9")])],
                                    quantifier=(0, None),
                                ),
                            ],
                        ),
                    ]
                )
            if self.minval == 0:
                # Lower bound is zero and no upper bound:
                #   [0, infinity)
                return Or(
                    [
                        # Zero: [0, 0]
                        String("0"),
                        # Positive: [1, infinity)
                        And(
                            [
                                CharRange([("1", "9")]),
                                And(
                                    [CharRange([("0", "9")])],
                                    quantifier=(0, None),
                                ),
                            ],
                        ),
                    ]
                )
            if self.minval == 1:
                # Lower bound is one and no upper bound:
                #   [1, infinity)
                return And(
                    [
                        CharRange([("1", "9")]),
                        And(
                            [CharRange([("0", "9")])],
                            quantifier=(0, None),
                        ),
                    ],
                )
            # Lower bound is greater than one and no upper bound:
            #   [minval, infinity) where minval > 1
            return self._unbounded_above_expr(self.minval)
        elif self.minval is None:
            self.maxval = cast(int, self.maxval)
            if self.maxval < 0:
                # No lower bound and upper bound is negative:
                #   (-infinity, maxval] where maxval < 0
                abs_hi = abs(self.maxval)
                return And(
                    [
                        String("-"),
                        # self._unbounded_above_expr(1) if abs_hi == 1 else self._nonnegative_range_expr(1, abs_hi),
                        self._unbounded_above_expr(abs_hi),
                    ],
                )
            if self.maxval == 0:
                # No lower bound and upper bound is zero:
                #   (-infinity, 0]
                return Or(
                    [
                        And(
                            [
                                String("-"),
                                CharRange([("1", "9")]),
                                And(
                                    [CharRange([("0", "9")])],
                                    quantifier=(0, None),
                                ),
                            ],
                        ),
                        String("0"),
                    ]
                )
            # No lower bound and upper bound is positive:
            #   (-infinity, maxval] where maxval > 0
            return Or(
                [
                    # Negative: (-infinity, -1]
                    And(
                        [
                            String("-"),
                            CharRange([("1", "9")]),
                            And(
                                [CharRange([("0", "9")])],
                                quantifier=(0, None),
                            ),
                        ],
                    ),
                    # Zero: [0, 0]
                    String("0"),
                    # Positive: [1, maxval]
                    self._nonnegative_range_expr(1, self.maxval),
                ],
            )

        if self.maxval < 0:
            # Lower and upper bounds are negative:
            #   [minval, maxval] where minval < 0 and maxval < 0
            abs_lo = abs(self.maxval)
            abs_hi = abs(self.minval)
            return And(
                [
                    String("-"),
                    self._nonnegative_range_expr(abs_lo, abs_hi),
                ]
            )

        if self.minval < 0 <= self.maxval:
            # Lower bound is negative and upper bound is non-negative:
            #   [minval, maxval] where minval < 0 <= maxval
            subexprs: list[Grammar] = []
            subexprs_n = 0
            abs_neg_lo = 1
            abs_neg_hi = abs(self.minval)
            if abs_neg_hi >= abs_neg_lo:
                neg_subexpr = And(
                    [
                        String("-"),
                        self._nonnegative_range_expr(abs_neg_lo, abs_neg_hi),
                    ]
                )
                subexprs.append(neg_subexpr)
                subexprs_n += 1
            if self.minval <= 0 <= self.maxval:
                zero_subexpr = String("0")
                subexprs.append(zero_subexpr)
                subexprs_n += 1
            if self.maxval > 0:
                pos_subexpr = self._nonnegative_range_expr(1, self.maxval)
                subexprs.append(pos_subexpr)
                subexprs_n += 1
            return Or(subexprs) if subexprs_n > 1 else subexprs[0]

        # Lower amd upper bounds are non-negative:
        #   [minval, maxval] where 0 <= minval <= maxval
        return self._nonnegative_range_expr(self.minval, self.maxval)

    def _nonnegative_range_expr(self, minval: int, maxval: int) -> Grammar:
        """Build an expression that matches all integers in [minval..maxval], assuming 0 <= minval <= maxval.

        - No leading zeros, unless the integer is "0".

        Args:
            minval (int): Lower bound of the range.
            maxval (int): Upper bound of the range.

        Returns:
            Grammar: Expression that matches all integers in the range.
        """
        assert 0 <= minval <= maxval

        if minval == maxval:
            return String(str(minval))

        subexprs: list[Grammar] = []
        # Explicitly handle when 0 is in range
        if minval <= 0 <= maxval:
            # zero_subexpr = String("0")
            # if 1 > maxval:  # If that was the entire range, done
            #     return zero_subexpr
            subexprs.append(String("0"))
            # new_min = 1
            minval = 1
        # else:
        #     new_min = minval

        # smin, smax = str(new_min), str(maxval)
        smin, smax = str(minval), str(maxval)
        len_min, len_max = len(smin), len(smax)
        for length in range(len_min, len_max + 1):
            block_lo = (10 ** (length - 1)) if length > 1 else 0
            block_hi = ((10**length) - 1) if length > 1 else 9
            # this_lo = max(block_lo, new_min)
            this_lo = max(block_lo, minval)
            this_hi = min(block_hi, maxval)
            if this_lo > this_hi:
                continue
            if (this_lo == block_lo) and (this_hi == block_hi):
                # Covers entire block of length
                subexprs.append(self._digits_expr(length))
            else:
                subexprs.append(self._same_quantifier_expr(str(this_lo), str(this_hi)))

        return Or(subexprs[::-1]) if len(subexprs) > 1 else subexprs[0]

    def _unbounded_above_expr(self, minval: int) -> Grammar:
        """Build an expression that matches all integers >= minval (for minval >= 1).

        Matches [minval, infinity) with no leading zeros.

        Args:
            minval (int): Lower bound of the range (must be >= 1).

        Returns:
            Grammar: Expression that matches all integers >= minval.
        """
        assert minval >= 1
        smin = str(minval)
        len_min = len(smin)
        subexprs = []
        # 1. Match numbers from minval to 10^len_min - 1 (same digit length as minval)
        block_hi = (10**len_min) - 1
        if minval <= block_hi:
            subexprs.append(self._same_quantifier_expr(smin, str(block_hi)))
        # 2. Match all numbers with len_min + 1 or more digits: [10^len_min, infinity)
        # This is: first digit [1-9], followed by len_min or more digits [0-9]
        # Pattern: [1-9][0-9]{len_min,}
        unbounded_part = And(
            [
                CharRange([("1", "9")]),
                And([CharRange([("0", "9")])], quantifier=(len_min, None)),
            ],
        )
        subexprs.append(unbounded_part)
        return Or(subexprs[::-1])

    @staticmethod
    def _digits_expr(k: int) -> Grammar:
        """For digit-length k >= 1, return an expression matching exactly k digits with no leading zero (unless k=1, which allows "0").

        Args:
            k (int): Number of digits to match.

        Returns:
            Grammar: Expression that matches exactly k digits with no leading zero.
        """
        if k < 1:
            raise ValueError(f"Invalid digit length: {k!r}")
        if k == 1:
            # [0-9]
            return CharRange([("0", "9")])
        if k == 2:
            # 10..99
            return And([CharRange([("1", "9")]), CharRange([("0", "9")])])
        # 100..999, 1000..9999, etc.
        return And(
            [
                CharRange([("1", "9")]),
                CharRange([("0", "9")]),
            ],
            quantifier=(k - 1, k - 1),
        )

    def _same_quantifier_expr(self, lo_str: str, hi_str: str) -> Grammar:
        """Create an expression matching all integers (no leading zeros) between lo_str and hi_str (inclusive).

        - Assumes len(lo_str) == len(hi_str) and lo_str <= hi_str.
        - No leading zeros are allowed unless the integer is "0".
        - Non-negative integers only.

        Args:
            lo_str (str): Lower bound (inclusive) as a decimal string.
            hi_str (str): Upper bound (inclusive) as a decimal string.

        Returns:
            Grammar: Expression that matches all integers between lo_str and hi_str.
        """
        if not (0 < len(lo_str) == len(hi_str)) or not (lo_str <= hi_str):
            raise ValueError(f"Invalid range: {lo_str!r} <= {hi_str!r}")
        # Single value
        if lo_str == hi_str:
            return String(lo_str)
        # Single-digit
        if len(lo_str) == 1:
            return CharRange([(lo_str, hi_str)])
        # Compare first digits
        first_lo, first_hi = lo_str[0], hi_str[0]
        rest_lo, rest_hi = lo_str[1:], hi_str[1:]
        if first_lo == first_hi:
            return And(
                [
                    String(first_lo),
                    self._same_quantifier_expr(rest_lo, rest_hi),
                ]
            )
        subexprs = []
        lo_digit = int(first_lo)
        hi_digit = int(first_hi)
        length = len(rest_lo)
        # 1. From first_lo + [rest_lo.."9"*length]
        subexprs.append(
            And(
                [
                    String(first_lo),
                    self._same_quantifier_expr(rest_lo, "9" * length),
                ],
            )
        )
        # 2. Middle digits if there's a gap of >= 2
        if hi_digit >= lo_digit + 2:
            if length == 1:
                subexprs.append(
                    And(
                        [
                            CharRange([(str(lo_digit + 1), str(hi_digit - 1))]),
                            CharRange([("0", "9")]),
                        ],
                    )
                )
            else:
                subexprs.append(
                    And(
                        [
                            CharRange([(str(lo_digit + 1), str(hi_digit - 1))]),
                            And([CharRange([("0", "9")])], quantifier=(length, length)),
                        ],
                    )
                )
        # 3. From first_hi + ["0"*length..rest_hi]
        subexprs.append(
            And(
                [
                    String(first_hi),
                    self._same_quantifier_expr("0" * length, rest_hi),
                ],
            )
        )
        return Or(subexprs[::-1])


class JSONIntegerLiteral(JSONComposition):
    """Composition that constructs a grammar to match a JSON integer literal.

    Args:
        value (int): Integer value to match.
    """

    __slots__: tuple[str, ...] = ("value",)

    def __init__(self, value: int) -> None:
        super().__init__()

        self.value: int = value
        """Integer value to match."""

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value} | super().attrs_dict()

    def grammar(self) -> String:
        """Construct a grammar for the composition.

        Returns:
            Grammar: Grammar for the composition.

        Examples:
            Create a composition and construct a grammar that matches the JSON integer value ``42``

            >>> from grammatica.builder.json_ import JSONIntegerLiteral
            >>> comp = JSONIntegerLiteral(value=42)
            >>> g = comp.grammar()
            >>> g
            String(value='42')
        """
        return String(str(self.value))
