#define _GNU_SOURCE
#include "grammatica/grammar/derivation_rule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
static void derivation_rule_destroy(GrammaticaGrammar* grammar);
static char* derivation_rule_render(const GrammaticaGrammar* grammar, bool full, bool wrap);
static GrammaticaGrammar* derivation_rule_simplify(const GrammaticaGrammar* grammar);
static GrammaticaGrammar* derivation_rule_copy(const GrammaticaGrammar* grammar);
static bool derivation_rule_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier);

static const GrammaticaGrammarVTable derivation_rule_vtable = {
    .destroy = derivation_rule_destroy,
    .render = derivation_rule_render,
    .simplify = derivation_rule_simplify,
    .copy = derivation_rule_copy,
    .equals = derivation_rule_equals,
};

GrammaticaDerivationRule* grammaticaDerivationRuleCreate(
    const char* symbol,
    GrammaticaGrammar* value
) {
    if (symbol == NULL || value == NULL) {
        return NULL;
    }
    GrammaticaDerivationRule* grammar = (GrammaticaDerivationRule*)malloc(sizeof(GrammaticaDerivationRule));
    if (grammar == NULL) {
        return NULL;
    }
    grammar->symbol = strdup(symbol);
    if (grammar->symbol == NULL) {
        free(grammar);
        return NULL;
    }
    grammar->value = grammaticaGrammarRef(value);
    grammar->base.type = GRAMMATICA_TYPE_DERIVATION_RULE;
    grammar->base.vtable = &derivation_rule_vtable;
    atomic_init(&grammar->base.refCount, 1);
    return grammar;
}

const char* grammaticaDerivationRuleGetSymbol(const GrammaticaDerivationRule* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    return grammar->symbol;
}

const GrammaticaGrammar* grammaticaDerivationRuleGetValue(const GrammaticaDerivationRule* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    return grammar->value;
}

static void derivation_rule_destroy(GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return;
    }
    GrammaticaDerivationRule* rule = (GrammaticaDerivationRule*)grammar;
    if (rule->symbol != NULL) {
        free(rule->symbol);
    }
    if (rule->value != NULL) {
        grammaticaGrammarUnref(rule->value);
    }
    free(rule);
}

static char* derivation_rule_render(const GrammaticaGrammar* grammar, bool full, bool wrap) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaDerivationRule* rule = (const GrammaticaDerivationRule*)grammar;
    if (!full) {
        return strdup(rule->symbol);
    }
    char* rendered_value = grammaticaGrammarRender(rule->value, false, wrap);
    if (rendered_value == NULL) {
        return NULL;
    }
    // Allocate buffer for symbol + " ::= " + rendered_value
    size_t total_len = strlen(rule->symbol) + strlen(" ::= ") + strlen(rendered_value) + 1;
    char* result = (char*)malloc(total_len);
    if (result == NULL) {
        free(rendered_value);
        return NULL;
    }
    snprintf(result, total_len, "%s ::= %s", rule->symbol, rendered_value);
    free(rendered_value);
    return result;
}

static GrammaticaGrammar* derivation_rule_simplify(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaDerivationRule* rule = (const GrammaticaDerivationRule*)grammar;
    GrammaticaGrammar* simplified_value = grammaticaGrammarSimplify(rule->value);
    if (simplified_value == NULL) {
        return NULL;
    }
    GrammaticaDerivationRule* result = grammaticaDerivationRuleCreate(
        rule->symbol,
        simplified_value
    );
    grammaticaGrammarUnref(simplified_value);
    return (GrammaticaGrammar*)result;
}

static GrammaticaGrammar* derivation_rule_copy(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaDerivationRule* rule = (const GrammaticaDerivationRule*)grammar;
    return (GrammaticaGrammar*)grammaticaDerivationRuleCreate(
        rule->symbol,
        rule->value
    );
}

static bool derivation_rule_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier) {
    if (grammar == other) {
        return true;
    }
    if (grammar == NULL || other == NULL) {
        return false;
    }
    if (grammar->type != other->type) {
        return false;
    }
    const GrammaticaDerivationRule* g1 = (const GrammaticaDerivationRule*)grammar;
    const GrammaticaDerivationRule* g2 = (const GrammaticaDerivationRule*)other;
    if (strcmp(g1->symbol, g2->symbol) != 0) {
        return false;
    }
    return grammaticaGrammarEquals(g1->value, g2->value, check_quantifier);
}
