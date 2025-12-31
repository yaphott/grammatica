from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComponent
from grammatica.grammar.base import Grammar
from grammatica.grammar.char_range import CharRange
from grammatica.grammar.group.and_ import And
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from typing import Any


class JSONInteger(JSONComponent):
    def __init__(
        self,
        minval: int | None = None,
        maxval: int | None = None,
    ) -> None:
        super().__init__()

        if (minval is not None) and (maxval is not None):
            if minval > maxval:
                raise ValueError(f"Invalid range: {minval!r} > {maxval!r}")
        self.minval: int | None = minval
        self.maxval: int | None = maxval

    def attrs_dict(self) -> dict[str, Any]:
        return {
            "minval": self.minval,
            "maxval": self.maxval,
        } | super().attrs_dict()

    def grammar(self) -> Grammar:
        if (self.minval is None) and (self.maxval is None):
            # Unbounded
            return Or(
                [
                    # Zero
                    String("0"),
                    # Non-zero
                    And(
                        [
                            # Optional negative sign
                            And([String("-")], quantifier=(0, 1)),
                            # First digit
                            CharRange([("1", "9")]),
                            # Optional subsequent digits
                            And([CharRange([("0", "9")])], quantifier=(0, None)),
                        ],
                    ),
                ]
            )
        if self.maxval is None:
            if self.minval == 0:
                return Or(
                    [
                        # Zero
                        String("0"),
                        # Positive
                        And(
                            [
                                # First digit
                                CharRange([("1", "9")]),
                                # Optional subsequent digits
                                And(
                                    [CharRange([("0", "9")])],
                                    quantifier=(0, None),
                                ),
                            ],
                        ),
                    ]
                )
            elif self.minval == 1:
                return And(
                    [
                        # First digit
                        CharRange([("1", "9")]),
                        # Optional subsequent digits
                        And([CharRange([("0", "9")])], quantifier=(0, None)),
                    ],
                )
            raise ValueError(
                "Feature not yet implemented: unbounded range with single non-zero bound"
            )
        if self.minval is None:
            if self.maxval == 0:
                return Or(
                    (
                        # Zero
                        String("0"),
                        # Negative
                        And(
                            (
                                String("-"),
                                # First digit
                                CharRange([("1", "9")]),
                                # Optional subsequent digits
                                And(
                                    [CharRange([("0", "9")])],
                                    quantifier=(0, None),
                                ),
                            ),
                        ),
                    )
                )
            raise ValueError(
                "Feature not yet implemented: unbounded range with single non-zero bound"
            )

        # Lower and upper bounds are negative
        if self.maxval < 0:
            # minval <= maxval < 0 => abs(minval) >= abs(maxval)
            abs_lo = abs(self.maxval)
            abs_hi = abs(self.minval)
            # e.g. neg_body could be "(?:2|3|...)" or just "5"
            # prefix with "-"
            neg_body = self._nonnegative_range_expr(abs_lo, abs_hi)
            if neg_body is None:
                raise ValueError(f"Invalid range: {self.minval!r} <= {self.maxval!r}")
            return And([String("-"), neg_body])

        # Lower bound is negative, upper bound is non-negative
        if self.minval < 0 <= self.maxval:
            subexprs: list[Grammar] = []

            # Negative
            abs_neg_lo = 1
            abs_neg_hi = abs(self.minval)
            if abs_neg_hi >= abs_neg_lo:
                neg_body = self._nonnegative_range_expr(abs_neg_lo, abs_neg_hi)
                if neg_body is not None:
                    neg_subexpr = And([String("-"), neg_body])
                    subexprs.append(neg_subexpr)

            # Positive
            if self.maxval > 0:
                pos_subexpr = self._nonnegative_range_expr(1, self.maxval)
                if pos_subexpr is not None:
                    subexprs.append(pos_subexpr)

            # Zero
            if self.minval <= 0 <= self.maxval:
                zero_subexpr = String("0")
                subexprs.append(zero_subexpr)

            # Combine
            if not subexprs:
                raise ValueError(f"Invalid range: {self.minval!r} <= {self.maxval!r}")
            return Or(subexprs) if len(subexprs) > 1 else subexprs[0]

        # Entirely non-negative
        nonneg_expr = self._nonnegative_range_expr(self.minval, self.maxval)
        if nonneg_expr is None:
            raise ValueError(f"Invalid range: {self.minval!r} <= {self.maxval!r}")
        return nonneg_expr

    def _nonnegative_range_expr(self, minval: int, maxval: int) -> Grammar | None:
        """Build an expression that matches all integers in [minval..maxval], assuming 0 <= minval <= maxval.

        - No leading zeros, unless the integer is "0".

        Args:
            minval (int): Lower bound of the range.
            maxval (int): Upper bound of the range.

        Returns:
            Grammar | None: Expression that matches all integers in the range.
        """
        assert 0 <= minval <= maxval

        if minval == maxval:
            return String(str(minval))

        appended_zero = False
        subexprs: list[Grammar] = []
        # If 0 is in range, handle '0' explicitly
        if minval <= 0 <= maxval:
            subexprs.append(String("0"))
            appended_zero = True
            new_min = 1
        else:
            new_min = minval

        # If that was the entire range, done
        if new_min > maxval:
            return String("0") if appended_zero else None

        smin, smax = str(new_min), str(maxval)
        len_min, len_max = len(smin), len(smax)

        for length in range(len_min, len_max + 1):
            block_lo = (10 ** (length - 1)) if length > 1 else 0
            block_hi = ((10**length) - 1) if length > 1 else 9

            this_lo = max(block_lo, new_min)
            this_hi = min(block_hi, maxval)
            if this_lo > this_hi:
                continue

            if (this_lo == block_lo) and (this_hi == block_hi):
                # Covers entire block of length
                subexprs.append(self._digits_expr(length))
            else:
                subexprs.append(self._same_quantifier_expr(str(this_lo), str(this_hi)))

        return Or(subexprs[::-1]) if len(subexprs) > 1 else subexprs[0]

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
            (
                CharRange([("1", "9")]),
                CharRange([("0", "9")]),
            ),
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


class JSONIntegerLiteral(JSONComponent):
    def __init__(self, value: int) -> None:
        super().__init__()

        self.value: int = value

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value} | super().attrs_dict()

    def grammar(self) -> String:
        return String(str(self.value))
