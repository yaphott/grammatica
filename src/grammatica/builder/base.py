"""Base composition and rule builder classes.

Provides abstractions for building grammars.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import TYPE_CHECKING, cast

from grammatica.base import Component

if TYPE_CHECKING:
    from typing import Any

    from grammatica.grammar.base import Grammar
    from grammatica.grammar.derivation_rule import DerivationRule


class Composition(Component, ABC):
    """Base class for compositions that build grammars."""

    # __slots__: tuple[str, ...] = ()

    def __init__(self) -> None:  # pylint: disable=W0246
        super().__init__()

    @abstractmethod
    def grammar(self) -> Grammar:
        """Construct a grammar for the composition.

        Returns:
            Grammar: Grammar for the composition.
        """

    @abstractmethod
    def attrs_dict(self) -> dict[str, Any]:
        return super().attrs_dict()

    def copy(self) -> Composition:
        """Create a copy of the composition.

        Returns:
            Composition: Copy of the composition.
        """
        return cast(Composition, super().copy())


class RuleBuilder(Component, ABC):
    """Base class for rule builders."""

    # __slots__: tuple[str, ...] = ()

    def __init__(self) -> None:  # pylint: disable=W0246
        super().__init__()

    @abstractmethod
    def build(self, value: Any, **kwargs) -> list[DerivationRule]:
        """Build the derivation rules for the given value.

        Args:
            value (Any): Value to build derivation rules for.
            **kwargs: Keyword arguments for the current context.

        Returns:
            list[DerivationRule]: Derivation rules.
        """

    def render(
        self,
        value: Any | Grammar | Composition,
    ) -> str:
        """Build the grammar and render it as a regular expression.

        Note:
            Returns empty :py:class:`str` if all rules render to None.

        Args:
            value (Any | Grammar | Composition): Value used to build the grammar.

        Returns:
            str: Rendered expression.
        """
        rules = self.build(value)
        rendered = ""
        for rule in rules:
            rendered_rule = rule.render(full=True, wrap=False)
            if rendered_rule is not None:
                rendered += rendered_rule + "\n"
        return rendered

    @abstractmethod
    def attrs_dict(self) -> dict[str, Any]:
        """Return instance attributes of the grammar builder.

        Returns:
            dict[str, Any]: Dictionary of instance attributes.
        """
        return {}

    def copy(self) -> RuleBuilder:
        """Create a copy of the grammar builder.

        Returns:
            RuleBuilder: Copy of the grammar builder.
        """
        return cast(RuleBuilder, super().copy())
