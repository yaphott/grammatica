"""Base grammar component and rule builder classes for building grammars.

Provides abstractions for building grammars.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

from grammatica.utils import value_to_string

if TYPE_CHECKING:
    from typing import Any

    from grammatica.grammar.base import Grammar
    from grammatica.grammar.derivation_rule import DerivationRule


class Component(ABC):
    """Base class for grammar components."""

    def __init__(self) -> None:
        pass

    @abstractmethod
    def attrs_dict(self) -> dict[str, Any]:
        """Return instance attributes of the component.

        Returns:
            dict[str, Any]: Dictionary of instance attributes.
        """
        return {}

    @abstractmethod
    def grammar(self) -> Grammar:
        """Construct a grammar for the component.

        Returns:
            Grammar: Grammar for the component.
        """

    def as_string(self, **kwargs) -> str:
        """Return a string representation of the grammar.

        Args:
            **kwargs: Keyword arguments for the current context.

        Returns:
            str: String representation of the grammar.

        Raises:
            ValueError: Attribute type is not supported.
        """
        attrs = self.attrs_dict()
        kwargs["indent"] = None
        msg = f"{type(self).__name__}("
        for j, (name, value) in enumerate(attrs.items()):
            if j > 0:
                msg += ", "
            msg += f"{name}="
            msg += value_to_string(value, **kwargs)
        msg += ")"
        return msg

    def __repr__(self) -> str:
        return self.__str__()

    def __str__(self) -> str:
        return self.as_string(indent=None)


class RuleBuilder(ABC):
    """Base class for rule builders."""

    def __init__(self) -> None:
        pass

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
        value: Any | Grammar | Component,
    ) -> str:
        """Build the grammar and render it as a regular expression.

        Note:
            Returns empty :py:class:`str` if all rules render to None.

        Args:
            value (Any | Grammar | Component): Value used to build the grammar.

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

    def as_string(self, **kwargs) -> str:
        """Return a string representation of the grammar builder.

        Args:
            **kwargs: Keyword arguments for the current context.

        Returns:
            str: String representation of the grammar builder.

        Raises:
            ValueError: Attribute type is not supported.
        """
        attrs = self.attrs_dict()
        kwargs["indent"] = None
        msg = f"{type(self).__name__}("
        for j, (name, value) in enumerate(attrs.items()):
            if j > 0:
                msg += ", "
            msg += f"{name}="
            msg += value_to_string(value, **kwargs)
        msg += ")"
        return msg

    def __repr__(self) -> str:
        return self.__str__()

    def __str__(self) -> str:
        return self.as_string(indent=None)
