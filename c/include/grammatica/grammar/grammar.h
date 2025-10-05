/**
 * @file grammar/grammar.h
 * @brief Grammar and Or (AND and OR operations)
 */

#ifndef GRAMMATICA_GRAMMAR_GRAMMAR_H
#define GRAMMATICA_GRAMMAR_GRAMMAR_H

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Grammar (AND) structure
 */
typedef struct {
    GrammaticaGrammar base;
    GrammaticaGrammarArray* subexprs;
    GrammaticaQuantifier quantifier;
} GrammaticaGroupedGrammar;

/**
 * @brief Create a new Grammar (AND) grammar
 * @param subexprs Array of subexpressions (will be copied and ref'd)
 * @param quantifier Quantifier
 * @return Pointer to the new grammar, or NULL on error
 */
GrammaticaGroupedGrammar* grammaticaGrammarCreate(
    const GrammaticaGrammarArray* subexprs,
    GrammaticaQuantifier quantifier
);

/**
 * @brief Create a new Or grammar
 * @param subexprs Array of subexpressions (will be copied and ref'd)
 * @param quantifier Quantifier
 * @return Pointer to the new grammar, or NULL on error
 */
GrammaticaGroupedGrammar* grammaticaOrCreate(
    const GrammaticaGrammarArray* subexprs,
    GrammaticaQuantifier quantifier
);

/**
 * @brief Get the subexpressions
 * @param grammar Grouped grammar
 * @return Array of subexpressions (do not modify or free)
 */
const GrammaticaGrammarArray* grammaticaGroupedGetSubexprs(const GrammaticaGroupedGrammar* grammar);

/**
 * @brief Get the quantifier
 * @param grammar Grouped grammar
 * @return Quantifier
 */
GrammaticaQuantifier grammaticaGroupedGetQuantifier(const GrammaticaGroupedGrammar* grammar);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_GRAMMAR_GRAMMAR_H */
