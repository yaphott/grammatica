"""
Classes and utilities for building JSON object grammar components.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.group.base import GroupJSONComponent
from grammatica.builder.json_.utils import build_json_grammar
from grammatica.grammar.group.and_ import And
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from collections.abc import Mapping
    from typing import Any

    from grammatica.builder.json_.base import JSONComponent
    from grammatica.grammar.base import Grammar


class JSONObject(GroupJSONComponent):
    """Component that matches a JSON object with specified key-value pairs.

    Args:
        value (Mapping[bool | int | float | str | None | Grammar | Component, bool | int | float | str | None | Grammar | Component]): Key-value pairs in the JSON object.
        item_ws (Grammar | None): Whitespace grammar between items.
        key_ws (Grammar | None): Whitespace grammar between keys and values.
    """

    def __init__(
        self,
        value: Mapping[
            bool | int | float | str | None | Grammar | JSONComponent,
            bool | int | float | str | None | Grammar | JSONComponent,
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
            bool | int | float | str | None | Grammar | JSONComponent,
            bool | int | float | str | None | Grammar | JSONComponent,
        ] = dict(value)

    def attrs_dict(self) -> dict[str, Any]:
        return super().attrs_dict() | {"value": self.value}

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
