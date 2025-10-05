/**
 * @file grammar/string.h
 * @brief String literal grammar
 */

#ifndef GRAMMATICA_GRAMMAR_STRING_H
#define GRAMMATICA_GRAMMAR_STRING_H

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief String grammar structure
 */
typedef struct {
    GrammaticaGrammar base;
    char* value;
} GrammaticaString;

/**
 * @brief Create a new string grammar
 * @param value String value (will be copied)
 * @return Pointer to the new grammar, or NULL on error
 */
GrammaticaString* grammaticaStringCreate(const char* value);

/**
 * @brief Get the string value
 * @param grammar String grammar
 * @return String value (do not modify or free)
 */
const char* grammaticaStringGetValue(const GrammaticaString* grammar);

/**
 * @brief Merge adjacent string grammars in an array (in-place)
 * @param array Grammar array
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError_t grammaticaMergeAdjacentStrings(GrammaticaGrammarArray* array);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_GRAMMAR_STRING_H */
