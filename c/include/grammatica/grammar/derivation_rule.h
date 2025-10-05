/**
 * @file grammar/derivation_rule.h
 * @brief Derivation rule grammar
 */

#ifndef GRAMMATICA_GRAMMAR_DERIVATION_RULE_H
#define GRAMMATICA_GRAMMAR_DERIVATION_RULE_H

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Derivation rule grammar structure
 */
typedef struct {
    GrammaticaGrammar base;
    char* symbol;
    GrammaticaGrammar* value;
} GrammaticaDerivationRule;

/**
 * @brief Create a new derivation rule grammar
 * @param symbol Symbol for the derivation rule (will be copied)
 * @param value Grammar the symbol derives into (will be ref'd)
 * @return Pointer to the new grammar, or NULL on error
 */
GrammaticaDerivationRule* grammaticaDerivationRuleCreate(
    const char* symbol,
    GrammaticaGrammar* value
);

/**
 * @brief Get the symbol
 * @param grammar Derivation rule grammar
 * @return Symbol (do not modify or free)
 */
const char* grammaticaDerivationRuleGetSymbol(const GrammaticaDerivationRule* grammar);

/**
 * @brief Get the value grammar
 * @param grammar Derivation rule grammar
 * @return Value grammar (do not modify or unref)
 */
const GrammaticaGrammar* grammaticaDerivationRuleGetValue(const GrammaticaDerivationRule* grammar);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_GRAMMAR_DERIVATION_RULE_H */
