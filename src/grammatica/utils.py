__all__ = [
    "char_to_cpoint",
    "char_to_hex",
    "ord_to_cpoint",
    "ord_to_hex",
]


def char_to_cpoint(char: str) -> str:
    return ord_to_cpoint(ord(char))


def ord_to_cpoint(ordinal: int) -> str:
    cpoint = format(ordinal, "X")
    if len(cpoint) < 5:
        return f"\\u{cpoint.zfill(4)}"
    return f"\\U{cpoint.zfill(8)}"


def char_to_hex(char: str) -> str:
    return ord_to_hex(ord(char))


def ord_to_hex(ordinal: int) -> str:
    return f"\\x{ordinal:02X}"
