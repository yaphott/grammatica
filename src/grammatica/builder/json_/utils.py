"""
Utility functions for JSON compositions and building grammars to match JSON values.
"""

from __future__ import annotations

import logging
from typing import TYPE_CHECKING

from grammatica.grammar.base import Grammar

if TYPE_CHECKING:
    from grammatica.builder.base import Composition
    from grammatica.builder.json_.base import JSONComposition
    from grammatica.builder.json_.type_aliases import JSONValue

logger: logging.Logger = logging.getLogger(__name__)


# TODO: Refactor so imports are not inside function bodies.
def coerce_to_json_composition(
    value: JSONValue,
    item_ws: Grammar | None,
    key_ws: Grammar | None,
) -> JSONComposition:
    from grammatica.builder.json_ import JSONBooleanLiteral
    from grammatica.builder.json_.group.array_ import JSONArrayLiteral
    from grammatica.builder.json_.group.object_ import JSONObject
    from grammatica.builder.json_.integer import JSONIntegerLiteral
    from grammatica.builder.json_.null import JSONNullLiteral

    # from grammatica.builder.json.float_ import JSONFloatLiteral
    from grammatica.builder.json_.string import JSONStringLiteral

    if value is None:
        return JSONNullLiteral()
    if isinstance(value, bool):
        return JSONBooleanLiteral(value)
    if isinstance(value, int):
        return JSONIntegerLiteral(value)
    # if isinstance(value, float):
    #     # TODO: Support other float formats. <---- This is a big one.
    #     return JSONFloatLiteral(value, item_ws=item_ws, key_ws=key_ws)
    if isinstance(value, str):
        # TODO: Add variations in string characters, like single quotes, backticks, etc.
        return JSONStringLiteral(value)
    if isinstance(value, list):
        return JSONArrayLiteral(value, item_ws=item_ws, key_ws=key_ws)
    if isinstance(value, dict):
        return JSONObject(value, item_ws=item_ws, key_ws=key_ws)
    raise ValueError(f"Invalid value type: {value!r}")


def build_json_grammar(
    value: JSONValue | Grammar | Composition,
    item_ws: Grammar | None,
    key_ws: Grammar | None,
) -> Grammar:
    from grammatica.builder.base import Composition
    from grammatica.builder.json_.group.base import GroupJSONComposition

    if isinstance(value, Grammar):
        return value.copy()
    if isinstance(value, GroupJSONComposition):
        if value.item_ws != item_ws:
            logger.warning("Updating item_ws: %r -> %r", value.item_ws, item_ws)
            value.item_ws = item_ws
        if value.key_ws != key_ws:
            logger.warning("Updating key_ws: %r -> %r", value.key_ws, key_ws)
            value.key_ws = key_ws
        return value.grammar()
    if isinstance(value, Composition):
        return value.grammar()
    coerced = coerce_to_json_composition(value, item_ws, key_ws)
    return coerced.grammar()
