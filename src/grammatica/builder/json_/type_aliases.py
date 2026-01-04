from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from typing import TypeAlias

    from grammatica.builder.base import Composition
    from grammatica.grammar.base import Grammar


__all__ = [
    "JSONCollection",
    "JSONPrimitive",
    "JSONValue",
]

JSONPrimitive: TypeAlias = str | int | float | bool | None
JSONCollection: TypeAlias = (
    list["JSONValue" | "Grammar" | "Composition"]
    | dict[
        "JSONValue" | "Grammar" | "Composition",
        "JSONValue" | "Grammar" | "Composition",
    ]
)
JSONValue: TypeAlias = JSONPrimitive | JSONCollection
