from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.grammar.base import Grammar
from grammatica.grammar.group.base import GroupGrammar
from grammatica.utils import value_to_string

if TYPE_CHECKING:
    from typing import Any


class NoOpGrammar(Grammar):
    def __init__(self):
        pass

    def render(self, **kwargs):
        return None

    def simplify(self):
        return None

    def attrs_dict(self):
        return {}


class NoOpGrammarAlt(Grammar):
    def __init__(self):
        pass

    def render(self, **kwargs):
        return None

    def simplify(self):
        return None

    def attrs_dict(self):
        return {}


class NoOpGroupGrammar(GroupGrammar):
    separator = " "

    def __init__(
        self,
        exprs,
        quantifier=None,
    ):
        super().__init__(
            subexprs=exprs,
            quantifier=quantifier if quantifier else (1, 1),
        )

    @staticmethod
    def simplify_subexprs(original_subexprs, quantifier):
        return None

    def needs_wrapped(self):
        return False

    def attrs_dict(self):  # pylint: disable=W0246
        return super().attrs_dict()


def fmt_result(result: Any, indent: int = 2) -> str:
    try:
        return value_to_string(result, indent=indent)
    except ValueError:
        return repr(result)
