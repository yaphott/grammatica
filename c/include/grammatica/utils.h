/**
 * @file utils.h
 * @brief Utility functions for character and ordinal conversions
 */

#ifndef GRAMMATICA_UTILS_H
#define GRAMMATICA_UTILS_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert a character to a Unicode code point string (\\uXXXX or \\UXXXXXXXX)
 * @param ch Character to convert
 * @param buffer Buffer to store the result (must be at least 11 bytes: \U + 8 hex + \0)
 * @param bufferSize Size of the buffer
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError Grammatica_charToCpoint(uint32_t ch, char* buffer, size_t bufferSize);

/**
 * @brief Convert an ordinal to a Unicode code point string (\\uXXXX or \\UXXXXXXXX)
 * @param ordinal Ordinal value to convert
 * @param buffer Buffer to store the result (must be at least 11 bytes: \U + 8 hex + \0)
 * @param bufferSize Size of the buffer
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError Grammatica_ordToCpoint(uint32_t ordinal, char* buffer, size_t bufferSize);

/**
 * @brief Convert a character to a hexadecimal escape sequence (\\xXX or \\xXXXX...)
 * @param ch Character to convert
 * @param buffer Buffer to store the result (must be at least 11 bytes)
 * @param bufferSize Size of the buffer
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError Grammatica_charToHex(uint32_t ch, char* buffer, size_t bufferSize);

/**
 * @brief Convert an ordinal to a hexadecimal escape sequence (\\xXX or \\xXXXX...)
 * @param ordinal Ordinal value to convert
 * @param buffer Buffer to store the result (must be at least 11 bytes)
 * @param bufferSize Size of the buffer
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError Grammatica_ordToHex(uint32_t ordinal, char* buffer, size_t bufferSize);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_UTILS_H */
