#include "utils.h"
#include <stdio.h>
#include <string.h>

GrammaticaError_t Grammatica_charToCpoint(uint32_t ch, char* buffer, size_t bufferSize) {
    return Grammatica_ordToCpoint(ch, buffer, bufferSize);
}

GrammaticaError_t Grammatica_ordToCpoint(uint32_t ordinal, char* buffer, size_t bufferSize) {
    if (buffer == NULL) {
        return GRAMMATICA_ERROR_NULL_POINTER;
    }
    if (ordinal < 0x10000) {
        // Use \\uXXXX format
        if (bufferSize < 7) {  // "\\u" + 4 hex digits + null terminator
            return GRAMMATICA_ERROR_BUFFER_TOO_SMALL;
        }
        snprintf(buffer, bufferSize, "\\u%04X", ordinal);
    } else {
        // Use \\UXXXXXXXX format
        if (bufferSize < 11) {  // "\\U" + 8 hex digits + null terminator
            return GRAMMATICA_ERROR_BUFFER_TOO_SMALL;
        }
        snprintf(buffer, bufferSize, "\\U%08X", ordinal);
    }
    return GRAMMATICA_OK;
}

GrammaticaError_t Grammatica_charToHex(uint32_t ch, char* buffer, size_t bufferSize) {
    return Grammatica_ordToHex(ch, buffer, bufferSize);
}

GrammaticaError_t Grammatica_ordToHex(uint32_t ordinal, char* buffer, size_t bufferSize) {
    if (buffer == NULL) {
        return GRAMMATICA_ERROR_NULL_POINTER;
    }
    if (bufferSize < 5) {  // "\\x" + at least 2 hex digits + null terminator (minimum)
        return GRAMMATICA_ERROR_BUFFER_TOO_SMALL;
    }
    snprintf(buffer, bufferSize, "\\x%02X", ordinal);
    return GRAMMATICA_OK;
}
