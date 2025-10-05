from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.grammar.base import value_to_string

if TYPE_CHECKING:
    from typing import Any


def fmt_result(result: Any, indent: int = 2) -> str:
    try:
        return value_to_string(result, indent=indent)
    except ValueError:
        return repr(result)
