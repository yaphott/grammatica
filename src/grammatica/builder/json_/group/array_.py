"""
Classes and utilities for building JSON array grammar components.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComponent
from grammatica.builder.json_.group.base import GroupJSONComponent
from grammatica.builder.json_.utils import build_json_grammar
from grammatica.grammar.group.and_ import And
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from collections.abc import Iterable
    from typing import Any

    from grammatica.grammar.base import Grammar


class JSONArray(GroupJSONComponent):
    """Component that matches a JSON array of values that conform to the provided grammar.

    Note:
        If the value of `value` is not an instance of ``Grammar`` or ``Component`` then it will be coerced into an instance of the appropriate corresponding JSON component.

    Args:
        value (bool | int | float | str | None | Grammar | Component): Grammar or value to match for each item in the JSON array.
        item_ws (Grammar | None): Whitespace grammar between items.
        key_ws (Grammar | None): Whitespace grammar between keys and values.
        n (int | tuple[int, int | None]): Minimum and maximum number of items in the array.

    See Also:
        :class:`grammatica.builder.json_.JSONArrayLiteral`: JSON array component with fixed size.
    """

    def __init__(
        self,
        value: bool | int | float | str | None | Grammar | JSONComponent,
        *,
        item_ws: Grammar | None = None,
        key_ws: Grammar | None = None,
        n: int | tuple[int, int | None] = (0, None),
    ) -> None:
        super().__init__(
            item_ws=item_ws,
            key_ws=key_ws,
            n=n,
        )

        self.value: bool | int | float | str | None | Grammar | JSONComponent = value

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value} | super().attrs_dict()

    def grammar(self) -> Grammar:
        # If the range is (0, 0), return an empty array
        if self.n[1] == 0:
            return String("[]")

        # Matches 1 or more values
        value_grammar = build_json_grammar(
            self.value,
            item_ws=self.item_ws,
            key_ws=self.key_ws,
        )
        subexprs: list[Grammar] = []

        initial_subexprs: list[Grammar] = []
        if self.item_ws is not None:
            initial_subexprs.append(self.item_ws)
        initial_subexprs.append(value_grammar)
        initial_grammar = And(initial_subexprs)
        subexprs.append(initial_grammar)

        if (self.n[1] is None) or (self.n[1] > 1):
            repeat_subexprs: list[Grammar] = []
            repeat_subexprs.append(String(","))
            if self.item_ws is not None:
                repeat_subexprs.append(self.item_ws)
            repeat_subexprs.append(value_grammar)
            repeat_quantifier = (
                self.n[0] - 1 if self.n[0] > 0 else 0,
                (self.n[1] - 1 if self.n[1] is not None else None),
            )
            repeat_grammar = And(repeat_subexprs, quantifier=repeat_quantifier)
            subexprs.append(repeat_grammar)
        if self.item_ws is not None:
            subexprs.append(self.item_ws)

        if self.n[0] == 0:
            # Wrap if optional
            return And([String("["), And(subexprs, quantifier=(0, 1)), String("]")])
        return And([String("["), *subexprs, String("]")])


class JSONArrayLiteral(GroupJSONComponent):
    """Component that matches a sized JSON array of values that conform to the provided grammars.

    Note:
        All values in `values` that are not an instance of ``Grammar`` or ``Component`` are coerced into an instance of the appropriate corresponding JSON component.

    Args:
        values (Iterable[bool | int | float | str | None | Grammar | Component]): Values in the JSON array.
        item_ws (Grammar | None): Whitespace grammar between items.
        key_ws (Grammar | None): Whitespace grammar between keys and values.

    See Also:
        :class:`grammatica.builder.json_.JSONArray`: JSON array component with flexible size.
    """

    def __init__(
        self,
        values: Iterable[bool | int | float | str | None | Grammar | JSONComponent],
        item_ws: Grammar | None = None,
        key_ws: Grammar | None = None,
    ) -> None:
        super().__init__(
            item_ws=item_ws,
            key_ws=key_ws,
            n=(1, 1),
        )

        self.values: list[bool | int | float | str | None | Grammar | JSONComponent] = (
            list(values)
        )

    def attrs_dict(self) -> dict[str, Any]:
        return {"values": self.values} | super().attrs_dict()

    def grammar(self) -> Grammar:
        if self.n[1] == 0:
            return String("[]")
        grammars: list[Grammar] = []
        grammars.append(String("["))
        for i, value in enumerate(self.values):
            if i > 0:
                grammars.append(String(","))
            if self.item_ws is not None:
                grammars.append(self.item_ws)
            value_grammar = build_json_grammar(
                value,
                item_ws=self.item_ws,
                key_ws=self.key_ws,
            )
            grammars.append(value_grammar)
        if self.item_ws is not None:
            grammars.append(self.item_ws)
        grammars.append(String("]"))
        return And(grammars)
