"""
Base component classes.
"""

from __future__ import annotations

from abc import ABC, abstractmethod
from typing import TYPE_CHECKING

from grammatica.utils import value_to_string

if TYPE_CHECKING:
    from typing import Any


class Component(ABC):
    """Base class for components."""

    __slots__: tuple[str, ...] = ()

    def __init__(self) -> None:
        pass

    @abstractmethod
    def attrs_dict(self) -> dict[str, Any]:
        """Return instance attributes of the component.

        Returns:
            dict[str, Any]: Dictionary of instance attributes.
        """
        return {}

    def copy(self) -> Component:
        """Create a copy of the component.

        Returns:
            Component: Copy of the component.
        """
        cls = type(self)
        kwargs = self.attrs_dict()
        component = cls(**kwargs)
        return component

    def as_string(self, **kwargs) -> str:
        """Return a string representation of the component.

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

    def equals(self, other: Any, **kwargs) -> bool:
        """Check equality with another value.

        Args:
            other (Any): Value to compare against.
            **kwargs: Keyword arguments for the current context.

        Returns:
            bool: True if the values are equal, False otherwise.
        """
        if self is other:
            return True
        if not isinstance(other, type(self)):
            return False
        attrs = self.attrs_dict()
        other_attrs = other.attrs_dict()
        return {k: attrs[k] for k in sorted(attrs)} == {
            k: other_attrs[k] for k in sorted(other_attrs)
        }

    def __eq__(self, other: Any) -> bool:
        return self.equals(other)
