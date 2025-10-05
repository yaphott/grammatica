"""
Grammatica is a GBNF (GGML BNF) grammar builder that exposes the full richness of the GBNF notation system using a syntax that reflects the resulting grammar.
"""

from importlib.metadata import PackageNotFoundError, version

__all__ = [
    "__version__",
]

try:
    __version__ = version(__name__)
except PackageNotFoundError:  # pragma: no cover
    __version__ = "0.0.0.dev0"
