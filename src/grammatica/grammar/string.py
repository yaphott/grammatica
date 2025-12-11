"""
Classes and utilities for string literal expressions.
"""

from __future__ import annotations

import sys
from typing import TYPE_CHECKING, cast

from grammatica.constants import (
    ALWAYS_SAFE_CHARS,
    CHAR_ESCAPE_MAP,
    STRING_LITERAL_ESCAPE_CHARS,
)
from grammatica.grammar.base import Grammar
from grammatica.utils import char_to_hex

if sys.version_info >= (3, 12):  # pragma: no cover
    from typing import override
else:  # pragma: no cover
    from typing_extensions import override

if TYPE_CHECKING:
    from collections.abc import Iterable
    from typing import Any


class String(Grammar):
    """Grammar that exactly matches a string.

    Args:
        value (Iterable[str]): Characters to match exactly.

    Examples:
        >>> from grammatica.grammar import String
        >>> g = String("gandalf")
        >>> g
        String(value='gandalf')
    """

    __slots__: tuple[str, ...] = ("value",)

    def __init__(self, value: Iterable[str]) -> None:
        super().__init__()

        self.value: str = str(value)
        """String to match exactly."""

    def render(self, **kwargs) -> str | None:
        """Render the grammar as a regular expression.

        Args:
            **kwargs: Keyword arguments for the current context.

        Returns:
            str | None: Rendered expression, or None if resolved to empty.

        Examples:
            Some characters are always safe and do not require escaping

            >>> from grammatica.grammar import String
            >>> g = String("gandalf")
            >>> print(g.render())
            "gandalf"

            Characters such as whitespace are escaped using a backslash (``\\``):

            >>> from grammatica.grammar import String
            >>> g = String("line\\nbreak")
            >>> print(g.render())
            "line\\nbreak"

            Otherwise characters are escaped to their hexadecimal representation

            >>> from grammatica.grammar import String
            >>> g = String("\\u6C34\\u706B")
            >>> print(g.render())
            "\\x6C34\\x706B"

            Empty strings are rendered as :py:obj:`None`

            >>> from grammatica.grammar import String
            >>> g = String("")
            >>> g.render() is None
            True
        """
        if len(self.value) == 0:
            return None
        return '"' + "".join(map(self._escape, self.value)) + '"'

    def simplify(self) -> String | None:
        """Simplify the grammar.

        Attempts to reduce redundancy, remove empty subexpressions, and optimize the grammar.

        Returns:
            String | None: Copy of the grammar, or None if resolved to empty.

        Examples:
            Non-empty strings remain unchanged

            >>> from grammatica.grammar import String
            >>> original = String("hello")
            >>> simplified = original.simplify()
            >>> original == simplified
            True
            >>> original is not simplified
            True

            Empty strings are simplified to :py:obj:`None`

            >>> from grammatica.grammar import String
            >>> g = String("")
            >>> g.simplify() is None
            True
        """
        if len(self.value) == 0:
            return None
        return String(self.value)

    @staticmethod
    def _escape(char: str) -> str:
        """Escape a character for use in a string literal.

        Args:
            char (str): Character to escape.

        Returns:
            str: Escaped character.
        """
        if char in ALWAYS_SAFE_CHARS:
            return char
        if char in CHAR_ESCAPE_MAP:
            return CHAR_ESCAPE_MAP[char]
        if char in STRING_LITERAL_ESCAPE_CHARS:
            return "\\" + char
        return char_to_hex(char)

    @override
    def as_string(self, indent: int | None = None) -> str:
        return super().as_string(indent=None)

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value}


def merge_adjacent_string_grammars(subexprs: list[Grammar], n: int) -> int:
    """Merge adjacent String grammars in-place. abc

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
        if isinstance(subexprs[i], String):
            if last_idx < 1:
                last_idx = i
            else:
                n -= 1
        else:
            if last_idx > 0:
                subexprs[i + 1] = String(
                    "".join(
                        cast(String, s).value for s in subexprs[i + 1 : last_idx + 1]
                    )
                )
                del subexprs[i + 2 : last_idx + 1]
            last_idx = -1

    if last_idx > 0:
        subexprs[0] = String(
            "".join(cast(String, s).value for s in subexprs[0 : last_idx + 1])
        )
        del subexprs[1 : last_idx + 1]

    return n
