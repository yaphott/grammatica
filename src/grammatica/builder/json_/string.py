from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComponent
from grammatica.builder.json_.constants import CHAR_ENCODE_MAP, PRINTABLE_CHARS
from grammatica.builder.json_.group.base import GroupJSONComponent
from grammatica.grammar.base import Grammar
from grammatica.grammar.char_range import CharRange
from grammatica.grammar.group.and_ import And
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from collections.abc import Iterable
    from typing import Any


_HEX_CHAR_RANGE: CharRange = CharRange([("0", "9"), ("A", "F"), ("a", "f")])
_ESCAPED_CHAR: Or = Or(
    [
        # Safe characters (excludes quote, backslash, and control characters)
        CharRange.from_ords(
            (
                [ord('"'), ord("\\")]  # Quote and backslash
                + list(range(0, 32))  # Control characters
                + [127]  # Control characters
            ),
            negate=True,
        ),
        # Escaped characters
        And(
            [
                String("\\"),
                Or(
                    [
                        CharRange.from_chars(
                            # Backslash
                            "\\"
                            # Forwardslash
                            + "/"
                            # Quote
                            + '"'
                            # Whitespace
                            + "bfnrt"
                        ),
                        # Unicode
                        And(
                            [
                                String("u"),
                                And([_HEX_CHAR_RANGE.copy()], quantifier=(4, 4)),
                            ]
                        ),
                    ],
                ),
            ]
        ),
    ],
)


class JSONString(GroupJSONComponent):
    def __init__(self, n: int | tuple[int, int | None] = (0, None)) -> None:
        super().__init__(
            item_ws=None,
            key_ws=None,
            n=n,
        )

    def attrs_dict(self) -> dict[str, Any]:  # pylint: disable=W0246
        return super().attrs_dict()

    def grammar(self) -> Grammar:
        return And(
            [
                String('"'),
                And([_ESCAPED_CHAR.copy()], quantifier=self.n),
                String('"'),
            ],
        )


class JSONStringLiteral(JSONComponent):
    """Component that matches a JSON string literal.

    Args:
        value (Iterable[str]): Characters of the string to match exactly.
        ensure_ascii (bool, optional): Whether to encode ASCII characters. Defaults to True.

    Examples:
        >>> from grammatica.builder.json_ import JSONStringLiteral
        >>> component = JSONStringLiteral("gandalf", ensure_ascii=False)
        >>> g = component.grammar()
        >>> g
        String(value='"gandalf"')
    """

    # TODO: Allow support for either escaped or non-escaped mix by providing `ensure_ascii=None`
    def __init__(self, value: Iterable[str], ensure_ascii: bool = True) -> None:
        super().__init__()

        self.value: str = "".join(value)
        self.ensure_ascii: bool = ensure_ascii

    def attrs_dict(self) -> dict[str, Any]:
        return {
            "value": self.value,
            "ensure_ascii": self.ensure_ascii,
        } | super().attrs_dict()

    @staticmethod
    def _encode_ascii(char: str) -> str:
        """Encode an ASCII-only JSON representation of a character.

        Args:
            text (str): Character to encode.

        Returns:
            str: Encoded character.
        """
        if char in PRINTABLE_CHARS:
            return char
        if char in CHAR_ENCODE_MAP:
            return CHAR_ENCODE_MAP[char]
        n = ord(char)
        if n < 0x10000:
            return f"\\u{n:04x}"
        n -= 0x10000
        s1 = 0xD800 | ((n >> 10) & 0x3FF)
        s2 = 0xDC00 | (n & 0x3FF)
        return f"\\u{s1:04x}\\u{s2:04x}"

    @staticmethod
    def _encode(char: str) -> str:
        """Encode a JSON representation of a character.

        Args:
            char (str): Character to encode.

        Returns:
            str: Encoded string.
        """
        if char in CHAR_ENCODE_MAP:
            return CHAR_ENCODE_MAP[char]
        return char

    def grammar(self) -> String:
        if not self.value:
            return String('""')
        encoded: str
        if self.ensure_ascii:
            encoded = "".join(map(self._encode_ascii, self.value))
        else:
            encoded = "".join(map(self._encode, self.value))
        return String('"' + encoded + '"')
