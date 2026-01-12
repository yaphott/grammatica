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

    def as_string(self, indent: int | None = None) -> str:
        """Return a string representation of the component.

        Args:
            indent (int, optional): Number of spaces to indent each level. Defaults to None.

        Returns:
            str: String representation of the component.

        Raises:
            ValueError: Attribute type is not supported.
        """
        attrs = self.attrs_dict()
        n = len(attrs)
        msg = f"{type(self).__name__}("
        for i, (name, value) in enumerate(attrs.items()):
            if indent is None:
                if i > 0:
                    msg += ", "
            else:
                if i > 0:
                    msg += ","
                msg += "\n" + (" " * indent)
            msg += f"{name}="
            if indent is None:
                msg += value_to_string(value, indent=indent)
            else:
                msg += value_to_string(value, indent=indent).replace(
                    "\n",
                    "\n" + (" " * indent),
                )
                if i == n - 1:
                    msg += "\n"
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
