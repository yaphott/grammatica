/**
 * @file constants.h
 * @brief Constants for character sets and escape mappings
 */

#ifndef GRAMMATICA_CONSTANTS_H
#define GRAMMATICA_CONSTANTS_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if a character is always safe (alphanumeric, punctuation, or space)
 * @param ch Character to check
 * @return true if the character is always safe, false otherwise
 */
bool grammaticaIsAlwaysSafeChar(uint32_t ch);

/**
 * @brief Get the escape sequence for a character if it has one (\\n, \\r, \\t)
 * @param ch Character to check
 * @param buffer Buffer to store the escape sequence (must be at least 3 bytes: \ + char + \0)
 * @return true if the character has an escape sequence, false otherwise
 */
bool grammaticaGetCharEscape(uint32_t ch, char* buffer);

/**
 * @brief Check if a character needs escaping in string literals
 * @param ch Character to check
 * @return true if the character needs escaping, false otherwise
 */
bool grammaticaIsStringLiteralEscapeChar(uint32_t ch);

/**
 * @brief Check if a character needs escaping in character ranges
 * @param ch Character to check
 * @return true if the character needs escaping, false otherwise
 */
bool grammaticaIsRangeEscapeChar(uint32_t ch);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_CONSTANTS_H */
