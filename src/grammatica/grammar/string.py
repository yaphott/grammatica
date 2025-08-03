"""
Classes and utilities for string literal expressions.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

try:
    from typing import override
except ImportError:
    from typing_extensions import override

if TYPE_CHECKING:
    from typing import Any
    from collections.abc import Iterable

from grammatica.constants import (
    ALWAYS_SAFE_CHARS,
    CHAR_ESCAPE_MAP,
    STRING_LITERAL_ESCAPE_CHARS,
)
from grammatica.grammar.base import BaseGrammar
from grammatica.utils import char_to_hex


def merge_adjacent_strings(subexprs: list[BaseGrammar], n: int) -> int:
    """Merge adjacent string grammars in-place.

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
        if isinstance(subexprs[i], String):
            if last_idx < 1:
                last_idx = i
            else:
                n -= 1
        else:
            if last_idx > 0:
                subexprs[i + 1] = String(
                    "".join(s.value for s in subexprs[i + 1 : last_idx + 1])
                )
                del subexprs[i + 2 : last_idx + 1]
            last_idx = -1

    if last_idx > 0:
        subexprs[0] = String("".join(s.value for s in subexprs[0 : last_idx + 1]))
        del subexprs[1 : last_idx + 1]

    return n


class String(BaseGrammar):
    """String literal that exactly matches a string."""

    def __init__(self, value: Iterable[str]) -> None:
        """Grammar that exactly matches a string.

        Args:
            value (Iterable[str]): String or sequence of characters for the string literal.
        """
        super().__init__()

        self.value: str = str(value)
        """String or sequence of characters for the string literal."""

    def render(self, full: bool = True, wrap: bool = True) -> str | None:
        if len(self.value) == 0:
            return None
        return '"' + "".join(map(self._escape, self.value)) + '"'

    def simplify(self) -> String | None:
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
