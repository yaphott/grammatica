from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.grammar.base import BaseGrammar

if TYPE_CHECKING:
    from typing import Any


def grammar_to_string(result: Any) -> str:
    if isinstance(result, BaseGrammar):
        return result.as_string(indent=2)
    return str(result)
