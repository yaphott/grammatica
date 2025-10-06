#include "constants.h"
#include <string.h>
#include <ctype.h>

bool grammaticaIsAlwaysSafeChar(uint32_t ch) {
    if (ch > 127) {
        return false;  // Only ASCII characters can be always safe
    }
    // Check if it's a digit (0-9)
    if (ch >= '0' && ch <= '9') {
        return true;
    }
    // Check if it's an ASCII letter (a-z, A-Z)
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
        return true;
    }
    // Check if it's punctuation: !#$%&'()*+,-./:;<=>?@[]^_`{|}~
    const char* punctuation = "!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
    if (strchr(punctuation, (char)ch) != NULL) {
        return true;
    }
    // Check if it's a space
    if (ch == ' ') {
        return true;
    }
    return false;
}

bool grammaticaGetCharEscape(uint32_t ch, char* buffer) {
    if (buffer == NULL) {
        return false;
    }
    switch (ch) {
        case '\n':
            buffer[0] = '\\';
            buffer[1] = 'n';
            buffer[2] = '\0';
            return true;
        case '\r':
            buffer[0] = '\\';
            buffer[1] = 'r';
            buffer[2] = '\0';
            return true;
        case '\t':
            buffer[0] = '\\';
            buffer[1] = 't';
            buffer[2] = '\0';
            return true;
        default:
            return false;
    }
}

bool grammaticaIsStringLiteralEscapeChar(uint32_t ch) {
    // Characters that need escaping in string literals: " and backslash
    return ch == '"' || ch == '\\';
}

bool grammaticaIsRangeEscapeChar(uint32_t ch) {
    // Characters that need escaping in character ranges: ^ - [ ] backslash
    return ch == '^' || ch == '-' || ch == '[' || ch == ']' || ch == '\\';
}
