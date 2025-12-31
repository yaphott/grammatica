from __future__ import annotations

import logging
from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

from grammatica.builder.base import Component, RuleBuilder
from grammatica.builder.json_.utils import build_json_grammar
from grammatica.grammar.base import Grammar
from grammatica.grammar.derivation_rule import DerivationRule
from grammatica.grammar.group.and_ import And
from grammatica.grammar.group.or_ import Or
from grammatica.grammar.string import String

if TYPE_CHECKING:
    from collections.abc import Iterable
    from typing import Any

logger: logging.Logger = logging.getLogger(__name__)


class JSONComponent(Component, ABC):
    @abstractmethod
    def attrs_dict(self) -> dict[str, Any]:
        return super().attrs_dict()

    @abstractmethod
    def grammar(self) -> Grammar: ...


DEFAULT_SPACE_GRAMMAR: Or = Or([String(" "), String("\t")])
DEFAULT_NEWLINE_GRAMMAR: Or = Or([String("\n"), String("\r\n")])
# RESERVED_NAMES: set[str] = {"root", "item-ws", "key-ws"}


# TODO: Add support for multiple different indent levels.
def item_ws_grammar(
    indent: int,
    allow_multiline: bool,
    space: Grammar,
    newline: Grammar,
    # TODO: Document performance impact of setting allow_compact to False.
    allow_compact: bool = True,
    # TODO: Document performance impact of setting max_level with both large and small values.
    max_level: int = -1,
    # TODO: `compact` will not be used here, instead whatever it calling `item_ws_grammar` will need to handle it and just not call.
    # This function would just return None if `compact` is True. So not having it here would be better.
    # compact: bool = False,
) -> Grammar:
    """Construct a whitespace grammar that separates items in JSON arrays and objects.

    Args:
        indent (int): Allowed count of space characters to use for each indentation level.
        allow_multiline (bool): Whether to allow multiline whitespace.
        space (Grammar): Grammar matching space characters.
        newline (Grammar): Grammar matching newline characters.
        allow_compact (bool): Whether to allow compact (no whitespace) formatting. Defaults to True.
        max_level (int): Maximum indentation level to support, or -1 if unlimited. Defaults to -1.

    Returns:
        Grammar: Whitespace grammar for separating JSON items.

    Raises:
        ValueError: Indent is non-positive.
    """
    # indents_list = sorted(set(indents))
    # if len(indents_list) < 1:
    #     raise ValueError("At least one indent level must be provided.")
    # if any(indent < 0 for indent in indents_list):
    #     raise ValueError(f"Indent levels must be non-negative: {indents_list}")
    # n_indents = len(indents_list)
    # indent_grammars = []
    # if n_indents < 2:
    #     indent = indents_list[0]
    #     if indent < 1:
    #         indent_grammars.append(space)
    #     else:
    #         indent_grammars.append(
    #             And(
    #                 [space],
    #                 quantifier=(
    #                     indent,
    #                     None if max_level < 0 else indent * max_level,
    #                 ),
    #             )
    #         )
    # else:
    #     for i in range(n_indents - 1, -1, -1):
    #         ...
    #         # indent = indents_list[i]
    #         # if indent < 1:
    #         #     indent_grammars.append(space)
    #         # else:
    #         #     indent_grammars.append(
    #         #         And(
    #         #             [space],
    #         #             quantifier=(
    #         #                 indent,
    #         #                 None if max_level < 0 else indent * max_level,
    #         #             ),
    #         #         )
    #         #     )
    if indent < 1:
        raise ValueError(f"Indent must be positive: {indent}")
    # if not compact:
    #     return None
    quantifier = (
        0 if allow_compact else 1,
        1,
    )
    allow_multiline &= (indent > 0) and (max_level != 0)
    if not allow_multiline:
        if quantifier == (1, 1):
            return space
        return And([space], quantifier=quantifier)
    indent_ws: Grammar
    if indent < 2:
        indent_ws = space
    else:
        indent_ws = And(
            [space],
            quantifier=(
                0,
                None if max_level < 0 else indent * max_level,
            ),
        )
    multiline_ws_grammar = And(
        [
            newline,
            And(
                [space],
                quantifier=(
                    0,
                    None if max_level < 0 else indent * max_level,
                ),
            ),
        ],
    )
    return Or(
        [multiline_ws_grammar, indent_ws],
        quantifier=quantifier,
    )


class JSONRuleBuilder(RuleBuilder):
    def __init__(
        self,
        *,
        indent: int = 2,
        max_level: int = 3,
        allow_multiline: bool = True,
        allow_compact: bool = True,
        compact: bool = False,
        space_chars: Iterable[str] = (" ", "\t"),
        newline_chars: Iterable[str] = ("\n",),
    ) -> None:
        super().__init__()

        self.indent: int = indent
        self.max_level: int = max_level
        self.allow_multiline: bool = allow_multiline
        self.allow_compact: bool = allow_compact
        self.compact: bool = compact

        space_chars_tuple = tuple(space_chars)
        space_chars_n = len(space_chars_tuple)
        if space_chars_n < 1:
            raise ValueError("space_chars must not be empty")
        self.space: Grammar
        if space_chars_n < 2:
            self.space = String(space_chars_tuple[0])
        else:
            self.space = Or(map(String, space_chars_tuple))

        newline_chars_tuple = tuple(newline_chars)
        newline_chars_n = len(newline_chars_tuple)
        if newline_chars_n < 1:
            raise ValueError("newline_chars must not be empty")
        self.newline: Grammar
        if newline_chars_n < 2:
            self.newline = String(newline_chars_tuple[0])
        else:
            self.newline = Or(map(String, newline_chars_tuple))

    def attrs_dict(self) -> dict[str, Any]:
        return {
            "indent": self.indent,
            "max_level": self.max_level,
            "allow_multiline": self.allow_multiline,
            "allow_compact": self.allow_compact,
            "compact": self.compact,
            "space": self.space,
            "newline": self.newline,
        } | super().attrs_dict()

    def build(
        self,
        value: bool | int | float | str | list | dict | None | Grammar | JSONComponent,
        **kwargs,
    ) -> list[DerivationRule]:
        rules = []
        item_ws_rule = None
        key_ws_rule = None
        if not self.compact:
            item_ws_rule = DerivationRule(
                "item-ws",
                item_ws_grammar(
                    indent=self.indent,
                    max_level=self.max_level,
                    allow_multiline=self.allow_multiline,
                    allow_compact=self.allow_compact,
                    space=self.space,
                    newline=self.newline,
                ),
            )
            rules.append(item_ws_rule)
            key_ws: And | String
            if self.allow_compact:
                key_ws = And([String(" ")], quantifier=(0, 1))
            else:
                key_ws = String(" ")
            key_ws_rule = DerivationRule("key-ws", key_ws)
            rules.append(key_ws_rule)
        root_rule = DerivationRule(
            "root",
            build_json_grammar(
                value,
                item_ws=item_ws_rule,
                key_ws=key_ws_rule,
            ),
        )
        rules.append(root_rule)
        return rules

    # def render(
    #     self,
    #     value: (
    #         bool | int | float | str | list | dict | None | Grammar | JSONComponent
    #     ),
    # ) -> str:
    #     return super().render(value)
