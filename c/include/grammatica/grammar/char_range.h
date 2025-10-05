/**
 * @file grammar/char_range.h
 * @brief Character range grammar
 */

#ifndef GRAMMATICA_GRAMMAR_CHAR_RANGE_H
#define GRAMMATICA_GRAMMAR_CHAR_RANGE_H

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Character range (inclusive)
 */
typedef struct {
    uint32_t start;
    uint32_t end;
} GrammaticaCharRangeEntry;

/**
 * @brief Character range grammar structure
 */
typedef struct {
    GrammaticaGrammar base;
    GrammaticaCharRangeEntry* ranges;
    size_t rangeCount;
    bool negate;
} GrammaticaCharRange;

/**
 * @brief Create a new character range grammar
 * @param ranges Array of character ranges
 * @param rangeCount Number of ranges
 * @param negate Negate the character range
 * @return Pointer to the new grammar, or NULL on error
 */
GrammaticaCharRange* grammaticaCharRangeCreate(
    const GrammaticaCharRangeEntry* ranges,
    size_t rangeCount,
    bool negate
);

/**
 * @brief Create a character range grammar from characters
 * @param chars Array of character code points
 * @param char_count Number of characters
 * @param negate Negate the character range
 * @return Pointer to the new grammar, or NULL on error
 */
GrammaticaCharRange* grammaticaCharRangeFromChars(
    const uint32_t* chars,
    size_t char_count,
    bool negate
);

/**
 * @brief Get the character ranges
 * @param grammar Character range grammar
 * @param rangeCount Output parameter for the number of ranges
 * @return Array of character ranges (do not modify or free)
 */
const GrammaticaCharRangeEntry* grammaticaCharRangeGetRanges(
    const GrammaticaCharRange* grammar,
    size_t* rangeCount
);

/**
 * @brief Check if the character range is negated
 * @param grammar Character range grammar
 * @return true if negated, false otherwise
 */
bool grammaticaCharRangeIsNegated(const GrammaticaCharRange* grammar);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_GRAMMAR_CHAR_RANGE_H */
