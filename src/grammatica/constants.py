__all__ = [
    "ALWAYS_SAFE_CHARS",
    "CHAR_ESCAPE_MAP",
    "RANGE_ESCAPE_CHARS",
    "STRING_LITERAL_ESCAPE_CHARS",
]

_DIGITS: frozenset[str] = frozenset(map(chr, b"0123456789"))
_ASCIILETTERS: frozenset[str] = frozenset(
    map(chr, b"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
)
_PUNCTUATION: frozenset[str] = frozenset(map(chr, b"!#$%&'()*+,-./:;<=>?@[]^_`{|}~"))
_SPACE: frozenset[str] = frozenset(map(chr, b" "))
ALWAYS_SAFE_CHARS: frozenset[str] = _DIGITS | _ASCIILETTERS | _PUNCTUATION | _SPACE
CHAR_ESCAPE_MAP: dict[str, str] = {
    "\n": "\\n",
    "\r": "\\r",
    "\t": "\\t",
}
STRING_LITERAL_ESCAPE_CHARS: frozenset[str] = frozenset(map(chr, b'"\\'))
RANGE_ESCAPE_CHARS: frozenset[str] = frozenset(map(chr, b"^-[]\\"))
