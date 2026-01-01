"""
Utility functions for building JSON grammar components.
"""

from __future__ import annotations

import logging
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from grammatica.builder.base import Component
    from grammatica.grammar.base import Grammar

logger: logging.Logger = logging.getLogger(__name__)


def build_json_grammar(
    value: bool | int | float | str | list | dict | None | Grammar | Component,
    item_ws: Grammar | None,
    key_ws: Grammar | None,
) -> Grammar:
    # TODO: Refactor and avoid circular imports.
    # from grammatica.builder.json.float_ import JSONFloatLiteral
    from grammatica.builder.json_.base import JSONComponent
    from grammatica.builder.json_.boolean import JSONBooleanLiteral
    from grammatica.builder.json_.group.array_ import JSONArrayLiteral
    from grammatica.builder.json_.group.base import GroupJSONComponent
    from grammatica.builder.json_.group.object_ import JSONObject
    from grammatica.builder.json_.integer import JSONIntegerLiteral
    from grammatica.builder.json_.null import JSONNullLiteral
    from grammatica.builder.json_.string import JSONStringLiteral
    from grammatica.grammar.base import Grammar

    g: Grammar
    if value is None:
        g = JSONNullLiteral().grammar()
    elif isinstance(value, JSONComponent):
        g = value.grammar()
    elif isinstance(value, bool):
        g = JSONBooleanLiteral(value).grammar()
    elif isinstance(value, int):
        g = JSONIntegerLiteral(value).grammar()
    # elif isinstance(value, float):
    #     # TODO: Support other float formats. <---- This is a big one.
    #     g = JSONFloatLiteral(value, item_ws=item_ws, key_ws=key_ws)
    elif isinstance(value, str):
        # TODO: Add variations in string characters, like single quotes, backticks, etc.
        g = JSONStringLiteral(value).grammar()
    elif isinstance(value, list):
        g = JSONArrayLiteral(value, item_ws=item_ws, key_ws=key_ws).grammar()
    elif isinstance(value, dict):
        g = JSONObject(value, item_ws=item_ws, key_ws=key_ws).grammar()
    elif isinstance(value, Grammar):
        g = value.copy()
    elif isinstance(value, GroupJSONComponent):
        if value.item_ws != item_ws:
            logger.warning("Updating item_ws: %r -> %r", value.item_ws, item_ws)
            value.item_ws = item_ws
        if value.key_ws != key_ws:
            logger.warning("Updating key_ws: %r -> %r", value.key_ws, key_ws)
            value.key_ws = key_ws
        g = value.grammar()
    # NOTE: Commented out as it is redundant due to the subsequent check for Component.
    # elif isinstance(value, JSONComponent):
    #     g = value.grammar()
    elif isinstance(value, Component):
        g = value.grammar()
    else:
        raise ValueError(f"Invalid value type: {value!r}")
    return g
