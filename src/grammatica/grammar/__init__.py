"""
Grammar classes and utility functions for handling grammar expressions.
"""

from grammatica.grammar.base import Grammar
from grammatica.grammar.char_range import CharRange
from grammatica.grammar.derivation_rule import DerivationRule
from grammatica.grammar.string import String, merge_adjacent_string_grammars

__all__ = [
    "Grammar",
    "CharRange",
    "DerivationRule",
    "String",
    "merge_adjacent_string_grammars",
]
