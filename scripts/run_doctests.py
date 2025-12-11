#!/usr/bin/env python3

from __future__ import annotations

import argparse
import doctest
import importlib
import inspect
import json
import logging
import pathlib
from collections.abc import Callable
from dataclasses import dataclass
from pathlib import Path
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from collections.abc import Iterator
    from typing import (
        Any,
        Literal,
        TextIO,
    )


PROJECT_DIR: Path = Path(__file__).resolve().parent.parent

logger: logging.Logger = logging.getLogger(__name__)


@dataclass
class Args:
    prefix: str | None
    raise_on_error: bool
    verbosity: int


def parse_args() -> Args:
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        "--prefix",
        type=str,
        default=None,
        help="Pattern for filtering which functions/methods to validate.",
    )
    parser.add_argument(
        "--raise-on-error",
        action="store_true",
        help="Raise an exception on the first error encountered.",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="count",
        dest="verbosity",
        default=0,
        help="Increase logging verbosity (0-2).",
    )
    args = parser.parse_args()
    return Args(
        prefix=args.prefix,
        raise_on_error=args.raise_on_error,
        verbosity=args.verbosity,
    )


def load_object(name: str) -> object:
    """Import an object using its full name.

    Args:
        name (str): Full name of the object to import (e.g., "module.submodule.Class.method").

    Returns:
        object: Imported object.

    Raises:
        ImportError: No module can be imported from the given name.
    """
    for maxsplit in range(name.count(".") + 1):
        module_name, *func_parts = name.rsplit(".", maxsplit)
        try:
            obj = importlib.import_module(module_name)
        except ImportError:
            pass
        else:
            break
    else:
        raise ImportError(f"No module can be imported from {name!r}")

    for part in func_parts:
        obj = getattr(obj, part)

    if isinstance(obj, Callable):
        obj = inspect.unwrap(obj)

    return obj


def get_api_items(fd: TextIO) -> Iterator[tuple[str, object, str, str]]:
    """Yield information about all public API items.

    Args:
        fd (TextIO): File descriptor of the API documentation page, containing the table of contents with all the public API.

    Yields:
        tuple[str, object, str, str]: Tuple containing the name of the object, the object itself, the section name, and the subsection name.
    """
    current_module = "grammatica"
    previous_line = current_section = current_subsection = ""
    position: Literal["autosummary", "items"] | None = None
    for line in fd:
        line_stripped = line.strip()
        if len(line_stripped) == len(previous_line):
            if set(line_stripped) == set("-"):
                current_section = previous_line
                continue
            if set(line_stripped) == set("~"):
                current_subsection = previous_line
                continue

        if line_stripped.startswith(".. currentmodule::"):
            current_module = line_stripped.removeprefix(".. currentmodule::").strip()
            continue

        if line_stripped == ".. autosummary::":
            position = "autosummary"
            continue

        if position == "autosummary":
            if line_stripped == "":
                position = "items"
                continue

        if position == "items":
            if line_stripped == "":
                position = None
                continue
            func_name = f"{current_module}.{line_stripped}"
            func = load_object(func_name)
            yield (
                func_name,
                func,
                current_section,
                current_subsection,
            )

        previous_line = line_stripped


def validate_all(
    prefix: str | None,
    raise_on_error: bool,
    verbose: bool,
) -> dict:
    """Execute and validate all docstring examples.

    Args:
        prefix (str | None): Prefix of object names to validate. If None, validate all objects.
        raise_on_error (bool, optional): Whether to raise an exception on the first error encountered.
        verbose (bool, optional): Whether to enable verbose output.

    Returns:
        dict: Validation results for each module.
    """
    result: dict[str, Any] = {}
    for obj_name, obj, section, subsection in get_all_api_items():
        if prefix and not obj_name.startswith(prefix):
            continue

        module = inspect.getmodule(obj)
        if module is None:
            # logger.warning("Unable to determine module for %s", func_name)
            continue
        module_name = module.__name__

        if module_name not in result:
            obj_file_str = inspect.getsourcefile(obj)
            if obj_file_str is None:
                logger.warning("Unable to determine source file for %r", obj_name)
                continue

            logger.debug("Testing docstring for %r", obj_name)
            n_failures, n_tries = doctest.testmod(
                m=module,
                verbose=verbose,
                name=module_name,
                optionflags=doctest.ELLIPSIS | doctest.NORMALIZE_WHITESPACE | doctest.DONT_ACCEPT_TRUE_FOR_1,
                raise_on_error=raise_on_error,
            )

            rel_file_path = Path(obj_file_str).relative_to(PROJECT_DIR)
            result[module_name] = {
                "file": str(rel_file_path),
                "n_failures": n_failures,
                "n_tries": n_tries,
                "referenced_sections": [],
                "public_apis": [],
            }

        section_key = (section, subsection or None)
        if section_key not in result[module_name]["referenced_sections"]:
            result[module_name]["referenced_sections"].append(section_key)

        if obj_name not in result[module_name]["public_apis"]:
            result[module_name]["public_apis"].append(obj_name)

    return result


def get_all_api_items():
    project_dir = pathlib.Path(__file__).parent.parent
    api_reference_dir = pathlib.Path(project_dir, "docs", "source", "api_reference")
    for path in api_reference_dir.rglob("*.rst"):
        with open(path, encoding="utf-8") as f:
            yield from get_api_items(f)


def main():
    args = parse_args()

    log_level: int
    if args.verbosity == 0:
        log_level = logging.ERROR
    elif args.verbosity == 1:
        log_level = logging.INFO
    elif args.verbosity == 2:
        log_level = logging.DEBUG
    else:
        raise ValueError(f"Invalid verbosity level: {args.verbosity}")
    logging.basicConfig(
        level=log_level,
        format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
    )

    result = validate_all(
        prefix=args.prefix,
        raise_on_error=args.raise_on_error,
        verbose=log_level <= logging.INFO,
    )
    print(json.dumps(result) + "\n", end="")


if __name__ == "__main__":
    main()
