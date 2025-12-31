from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.builder.json_.base import JSONComponent
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from typing import Any

    from grammatica.grammar.base import Grammar


class JSONBoolean(JSONComponent):
    def __init__(self) -> None:  # pylint: disable=W0246
        super().__init__()

    def attrs_dict(self) -> dict[str, Any]:  # pylint: disable=W0246
        return super().attrs_dict()

    def grammar(self) -> Or:
        return Or([String("true"), String("false")])


class JSONBooleanLiteral(JSONComponent):
    def __init__(self, value: bool) -> None:
        self.value: bool = value

    def attrs_dict(self) -> dict[str, Any]:
        return {"value": self.value} | super().attrs_dict()

    def grammar(self) -> Grammar:
        return String("true" if self.value else "false")
