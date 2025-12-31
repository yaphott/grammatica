from __future__ import annotations

import json
from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComponent
from grammatica.builder.json_.group.base import GroupJSONComponent
from grammatica.grammar.base import Grammar
from grammatica.grammar.char_range import CharRange
from grammatica.grammar.group.and_ import And
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

if TYPE_CHECKING:
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
                        # Or(
                        #     [
                        #         And([String("u"), HEX_CHAR_RANGE], length_range=4),
                        #         And([String("U"), HEX_CHAR_RANGE], length_range=8),
                        #     ],
                        # ),
                    ],
                ),
            ]
        ),
    ],
)


# TODO: Add support for character ranges.
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
                # self.quote,
                # Grammar([self.escaped_char], length_range=self.length_range),
                # self.quote,
                String('"'),
                And([_ESCAPED_CHAR.copy()], quantifier=self.n),
                String('"'),
            ],
        )


# TODO: Add support for character ranges.
class JSONStringLiteral(JSONComponent):
    def __init__(self, value: str, ensure_ascii: bool = True) -> None:
        super().__init__()

        self.value: str = value
        self.ensure_ascii: bool = ensure_ascii

    def attrs_dict(self) -> dict[str, Any]:
        return {
            "value": self.value,
            "ensure_ascii": self.ensure_ascii,
        } | super().attrs_dict()

    def grammar(self) -> String:
        serialized_str = json.dumps(self.value, ensure_ascii=self.ensure_ascii)
        return String(serialized_str)
