from __future__ import annotations

from typing import TYPE_CHECKING

from grammatica.grammar.base import BaseGrammar, value_to_string
from grammatica.grammar.base_group import BaseGroupGrammar

if TYPE_CHECKING:
    from typing import Any


class NoOpGrammar(BaseGrammar):
    def __init__(self):
        pass

    def render(self, wrap=False):
        return self.render()

    def simplify(self):
        return self.simplify()

    def attrs_dict(self):
        return {}


class NoOpGrammarAlt(BaseGrammar):
    def __init__(self):
        pass

    def render(self, wrap=False):
        return self.render()

    def simplify(self):
        return self.simplify()

    def attrs_dict(self):
        return {}


class NoOpGroupGrammar(BaseGroupGrammar):
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

    def attrs_dict(self):
        return super().attrs_dict()


def fmt_result(result: Any, indent: int = 2) -> str:
    try:
        return value_to_string(result, indent=indent)
    except ValueError:
        return repr(result)
