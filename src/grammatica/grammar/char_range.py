"""
Classes and utilities for character range expressions that define sets of allowed or disallowed characters.
"""

from __future__ import annotations

import sys
from typing import TYPE_CHECKING

from grammatica.constants import (
    ALWAYS_SAFE_CHARS,
    CHAR_ESCAPE_MAP,
    RANGE_ESCAPE_CHARS,
)
from grammatica.grammar.base import Grammar
from grammatica.grammar.string import String
from grammatica.utils import char_to_hex

if sys.version_info >= (3, 12):  # pragma: no cover
    from typing import override
else:  # pragma: no cover
    from typing_extensions import override

if TYPE_CHECKING:
    from collections.abc import Iterable, Iterator
    from typing import Any


class CharRange(Grammar):
    """Grammar that defines a set of allowed or disallowed characters.

    Args:
        char_ranges (Iterable[tuple[str, str]]): Character ranges in the form of tuples (start, end).
            Each range is inclusive, meaning both start and end characters are included.
            For example, ('a', 'z') includes all lowercase letters from 'a' to 'z'.
        negate (bool, optional): Negate the character range. Defaults to False.

    Raises:
        ValueError: Empty character range provided.
        ValueError: Start or end of a character range is not a single character.
        ValueError: End character is less than start character in a character range.
    """

    __slots__: tuple[str, ...] = ("char_ranges", "negate")

    def __init__(
        self,
        char_ranges: Iterable[tuple[str, str]],
        negate: bool = False,
    ) -> None:
        super().__init__()

        unsorted_char_ranges = list(char_ranges)
        if len(unsorted_char_ranges) == 0:
            raise ValueError("char_ranges must not be empty")
        for start, end in unsorted_char_ranges:
            if len(start) != 1:
                raise ValueError("start must be a single character")
            if len(end) != 1:
                raise ValueError("end must be a single character")
            if end < start:
                raise ValueError("end must be greater than or equal to start")
        ords: set[int] = set()
        for cstart, cend in unsorted_char_ranges:
            ords.update(range(ord(cstart), ord(cend) + 1))
        ord_ranges = self._iter_ords_to_ord_ranges(ords)
        self.char_ranges: list[tuple[str, str]] = list(
            map(self._ord_range_to_char_range, ord_ranges)
        )
        """Character ranges in the form of tuples (start, end).
            Each range is inclusive, meaning both start and end characters are included.
            For example, ('a', 'z') includes all lowercase letters from 'a' to 'z'.
        """
        self.negate: bool = negate
        """Negate the character range."""

    def render(self, full: bool = True, wrap: bool = True) -> str | None:
        if len(self.char_ranges) == 0:
            return None
        # TODO: Should character ranges be validated before rendering? They're currently only validated upon construction.
        ords: set[int] = set()
        for cstart, cend in self.char_ranges:
            ords.update(range(ord(cstart), ord(cend) + 1))
        expr = "["
        if self.negate:
            expr += "^"
        for start, end in self._iter_ords_to_ord_ranges(ords):
            start_esc = self._escape(chr(start))
            if start == end:
                expr += start_esc
                continue
            end_esc = self._escape(chr(end))
            if end == start + 1:
                expr += f"{start_esc}{end_esc}"
            else:
                expr += f"{start_esc}-{end_esc}"
        expr += "]"
        return expr

    def simplify(self) -> CharRange | String | None:
        n = len(self.char_ranges)
        if n == 0:
            return None
        if (n == 1) and (self.char_ranges[0][0] == self.char_ranges[0][1]):
            return String(self.char_ranges[0][0])
        return CharRange(self.char_ranges.copy(), negate=self.negate)

    @staticmethod
    def _escape(char: str) -> str:
        """Escape a character for use in a character range.

        Args:
            char (str): Character to escape.

        Returns:
            str: Escaped character.
        """
        if char in RANGE_ESCAPE_CHARS:
            return "\\" + char
        if char in ALWAYS_SAFE_CHARS:
            return char
        if char in CHAR_ESCAPE_MAP:
            return CHAR_ESCAPE_MAP[char]
        return char_to_hex(char)

    @classmethod
    def from_chars(cls, chars: Iterable[str], negate: bool = False) -> CharRange:
        """Create an instance of :class:`grammatica.grammar.CharRange` from an iterable of characters.

        Note:
            Duplicate characters in the input are ignored.

        Args:
            chars (Iterable[str]): Characters to include in the character ranges.
            negate (bool, optional): Negate the character ranges. Defaults to False.

        Returns:
            CharRange: Instance created from the provided characters.

        Raises:
            ValueError: No characters provided.
        """
        return cls.from_ords(map(ord, chars), negate=negate)

    @staticmethod
    def _ord_range_to_char_range(ord_range: tuple[int, int]) -> tuple[str, str]:
        """Convert a range of ordinals to a range of characters.

        Args:
            ord_range (tuple[int, int]): Ordinal range in the form of (start, end).
                The range is inclusive, meaning both start and end ordinals are included.
                For example, (97, 122) includes all lowercase letters from 'a' to 'z'.

        Returns:
            tuple[str, str]: Character range corresponding to the provided ordinal range.

        Raises:
            ValueError: Invalid ordinal(s) in ordinal range.
        """
        return chr(ord_range[0]), chr(ord_range[1])

    @classmethod
    def from_ords(cls, ords: Iterable[int], negate: bool = False) -> CharRange:
        """Create an instance of :class:`grammatica.grammar.CharRange` from an iterable of ordinals.

        Note:
            Duplicate ordinals in the input are ignored.

        Args:
            ords (Iterable[int]): Ordinals to include in the character ranges.
            negate (bool, optional): Negate the character ranges. Defaults to False.

        Returns:
            CharRange: Instance created from the provided ordinals.

        Raises:
            ValueError: No ordinals provided.
        """
        ord_ranges = cls._iter_ords_to_ord_ranges(ords)
        char_ranges = map(cls._ord_range_to_char_range, ord_ranges)
        return cls(char_ranges, negate=negate)

    @staticmethod
    def _iter_ords_to_ord_ranges(ords: Iterable[int]) -> Iterator[tuple[int, int]]:
        """Generate ranges of consecutive ordinals from an iterable of ordinals.

        Args:
            ords (Iterable[int]): Ordinals to generate ordinal ranges from.

        Yields:
            tuple[int, int]: Ordinal range in the form of (start, end).
                The range is inclusive, meaning both start and end ordinals are included.
                For example, (97, 122) includes all lowercase letters from 'a' to 'z'.

        Raises:
            ValueError: No ordinals provided.
        """
        sorted_ords = sorted(set(ords))
        start = sorted_ords[0]
        end = start
        for i in range(1, len(sorted_ords)):
            ord_ = sorted_ords[i]
            if ord_ != end + 1:
                yield start, end
                start = ord_
            end = ord_
        yield start, end

    @override
    def as_string(self, indent: int | None = None) -> str:
        return super().as_string(indent=None)

    def attrs_dict(self) -> dict[str, Any]:
        return {
            "char_ranges": self.char_ranges,
            "negate": self.negate,
        }
