import pytest

from grammatica.grammar.base import BaseGroupGrammar


class StubGroupGrammar(BaseGroupGrammar):
    def __init__(
        self,
        exprs,
        length_range=None,
    ):
        super().__init__(
            subexprs=exprs,
            length_range=length_range,
        )

    @classmethod
    def simplify_subexprs(cls, original_subexprs, length_range):
        return None

    def needs_wrapped(self):
        return False

    def attrs_dict(self):
        return super().attrs_dict()


@pytest.mark.parametrize(
    "length_range, expected",
    [
        ((1, 1), None),
        ((0, 1), "?"),
        ((0, 2), "{,2}"),
        ((1, None), "+"),
        ((2, None), "{2,}"),
        ((2, 2), "{2}"),
        ((2, 3), "{2,3}"),
    ],
)
def test_group_grammar_render_quantifier(length_range, expected):
    grammar = StubGroupGrammar([], length_range=length_range)
    assert grammar.render_quantifier() == expected
