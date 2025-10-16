#include "grammatica.h"
#include "grammatica_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const char* DERIVATION_RULE_SEPARATOR = " ::= ";

/* Create DerivationRule */
DerivationRule* grammatica_derivation_rule_create(GrammaticaContextHandle_t ctx, const char* symbol, Grammar* value) {
    if (!ctx || !symbol || !value) {
        return NULL;
    }
    DerivationRule* rule = (DerivationRule*)calloc(1, sizeof(DerivationRule));
    if (!rule) {
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    rule->symbol = strdup(symbol);
    if (!rule->symbol) {
        free(rule);
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    rule->value = value;
    return rule;
}

/* Destroy DerivationRule */
void grammatica_derivation_rule_destroy(GrammaticaContextHandle_t ctx, DerivationRule* rule) {
    if (!rule) {
        return;
    }
    free(rule->symbol);
    if (rule->value) {
        grammatica_grammar_destroy(ctx, rule->value);
    }
    free(rule);
}

/* Render DerivationRule */
char* grammatica_derivation_rule_render(GrammaticaContextHandle_t ctx, const DerivationRule* rule, bool full, bool wrap) {
    if (!ctx || !rule) {
        return NULL;
    }
    if (!full) {
        return strdup(rule->symbol);
    }
    char* rendered = grammatica_grammar_render(ctx, rule->value, false, wrap);
    if (!rendered) {
        return NULL;
    }
    char* result = (char*)malloc(strlen(rule->symbol) + strlen(DERIVATION_RULE_SEPARATOR) + strlen(rendered) + 1);
    if (!result) {
        grammatica_free_string(ctx, rendered);
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    sprintf(result, "%s%s%s", rule->symbol, DERIVATION_RULE_SEPARATOR, rendered);
    grammatica_free_string(ctx, rendered);
    return result;
}

/* Simplify DerivationRule */
Grammar* grammatica_derivation_rule_simplify(GrammaticaContextHandle_t ctx, const DerivationRule* rule) {
    if (!ctx || !rule) {
        return NULL;
    }
    Grammar* simplified = grammatica_grammar_simplify(ctx, rule->value);
    if (!simplified) {
        return NULL;
    }
    DerivationRule* new_rule = grammatica_derivation_rule_create(ctx, rule->symbol, simplified);
    if (!new_rule) {
        grammatica_grammar_destroy(ctx, simplified);
        return NULL;
    }
    Grammar* grammar = (Grammar*)malloc(sizeof(Grammar));
    if (!grammar) {
        grammatica_derivation_rule_destroy(ctx, new_rule);
        return NULL;
    }
    grammar->type = GRAMMAR_TYPE_DERIVATION_RULE;
    grammar->data = new_rule;
    return grammar;
}

/* Convert DerivationRule to string representation */
char* grammatica_derivation_rule_as_string(GrammaticaContextHandle_t ctx, const DerivationRule* rule) {
    if (!ctx || !rule) {
        return NULL;
    }
    char* value_str = grammatica_grammar_as_string(ctx, rule->value);
    if (!value_str) {
        return NULL;
    }
    char* result = (char*)malloc(4096);
    if (!result) {
        grammatica_free_string(ctx, value_str);
        return NULL;
    }
    snprintf(result, 4096, "DerivationRule(symbol='%s', value=%s)", rule->symbol, value_str);
    grammatica_free_string(ctx, value_str);
    char* final_result = strdup(result);
    free(result);
    return final_result;
}

/* Check if two DerivationRules are equal */
bool grammatica_derivation_rule_equals(GrammaticaContextHandle_t ctx, const DerivationRule* a, const DerivationRule* b) {
    if (!ctx) {
        return false;
    }
    if (a == b) {
        return true;
    }
    if (!a || !b) {
        return false;
    }
    if (strcmp(a->symbol, b->symbol) != 0) {
        return false;
    }
    return grammatica_grammar_equals(ctx, a->value, b->value);
}

/* Copy DerivationRule */
DerivationRule* grammatica_derivation_rule_copy(GrammaticaContextHandle_t ctx, const DerivationRule* rule) {
    if (!ctx || !rule) {
        return NULL;
    }
    Grammar* value_copy = grammatica_grammar_copy(ctx, rule->value);
    if (!value_copy) {
        return NULL;
    }
    return grammatica_derivation_rule_create(ctx, rule->symbol, value_copy);
}

/* Get symbol */
const char* grammatica_derivation_rule_get_symbol(GrammaticaContextHandle_t ctx, const DerivationRule* rule) {
    if (!ctx || !rule) {
        return NULL;
    }
    return rule->symbol;
}

/* Get value */
const Grammar* grammatica_derivation_rule_get_value(GrammaticaContextHandle_t ctx, const DerivationRule* rule) {
    if (!ctx || !rule) {
        return NULL;
    }
    return rule->value;
}
