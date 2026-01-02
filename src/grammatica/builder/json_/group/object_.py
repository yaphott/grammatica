"""
Classes and utilities for grouped compositions that construct a grammar to match a JSON object.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.group.base import GroupJSONComposition
from grammatica.builder.json_.utils import build_json_grammar
from grammatica.grammar.group.and_ import And
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from collections.abc import Mapping
    from typing import Any

    from grammatica.builder.base import Composition
    from grammatica.grammar.base import Grammar


class JSONObject(GroupJSONComposition):
    """Composition that matches a JSON object with specified key-value pairs.

    Args:
        value (Mapping[bool | int | float | str | None | Grammar | Composition, bool | int | float | str | None | Grammar | Composition]): Key-value pairs in the JSON object.
        item_ws (Grammar | None): Whitespace grammar between items.
        key_ws (Grammar | None): Whitespace grammar between keys and values.

    Raises:
        ValueError: Range lower bound is negative.
        ValueError: Range upper bound is not positive or None (infinity).
        ValueError: Range lower bound is greater than range upper bound.

    Examples:
        >>> from grammatica.builder.json_ import JSONObject
        >>> comp = JSONObject({
        ...     "name": "Samwise Gamgee",
        ...     "age": 38,
        ...     "is_hobbit": True,
        ... })
        >>> comp
        JSONObject(value={'name': 'Samwise Gamgee', 'age': 38, 'is_hobbit': True}, n=(1, 1), item_ws=None, key_ws=None)
        >>> g = comp.grammar()
        >>> print(g.as_string(indent=4))
        And(
            subexprs=[
                String(value='{'),
                String(value='"name"'),
                String(value=':'),
                String(value='"Samwise Gamgee"'),
                String(value=','),
                String(value='"age"'),
                String(value=':'),
                String(value='38'),
                String(value=','),
                String(value='"is_hobbit"'),
                String(value=':'),
                String(value='true'),
                String(value='}')
            ],
            quantifier=(1, 1)
        )
    """

    def __init__(
        self,
        value: Mapping[
            bool | int | float | str | None | Grammar | Composition,
            bool | int | float | str | None | Grammar | Composition,
        ],
        item_ws: Grammar | None = None,
        key_ws: Grammar | None = None,
    ) -> None:
        super().__init__(
            item_ws=item_ws,
            key_ws=key_ws,
            n=(1, 1),
        )

        self.value: dict[
            bool | int | float | str | None | Grammar | Composition,
            bool | int | float | str | None | Grammar | Composition,
        ] = dict(value)
        """Key-value pairs in the JSON object."""

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value} | super().attrs_dict()

    def grammar(self) -> Grammar:
        if self.n[1] == 0:
            return String("{}")

        if self.n[1] == 0:
            return String("{}")

        grammars: list[Grammar] = []
        grammars.append(String("{"))
        for i, (key, value) in enumerate(self.value.items()):
            if i > 0:
                grammars.append(String(","))
            if self.item_ws is not None:
                grammars.append(self.item_ws)
            key_grammar = build_json_grammar(
                key,
                item_ws=self.item_ws,
                key_ws=self.key_ws,
            )
            grammars.append(key_grammar)
            grammars.append(String(":"))
            if self.key_ws is not None:
                grammars.append(self.key_ws)
            value_grammar = build_json_grammar(
                value,
                item_ws=self.item_ws,
                key_ws=self.key_ws,
            )
            grammars.append(value_grammar)
        if self.item_ws is not None:
            grammars.append(self.item_ws)
        grammars.append(String("}"))
        return And(grammars)
