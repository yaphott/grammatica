__all__ = [
    "CHAR_ENCODE_MAP",
    "PRINTABLE_CHARS",
]

PRINTABLE_CHARS: frozenset[str] = frozenset(map(chr, range(32, 127)))
CHAR_ENCODE_MAP: dict[str, str] = {
    "\\": "\\\\",
    '"': '\\"',
    "\b": "\\b",
    "\f": "\\f",
    "\n": "\\n",
    "\r": "\\r",
    "\t": "\\t",
} | {chr(i): f"\\u{i:04x}" for i in range(0x20)}
