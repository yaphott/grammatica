#include "grammar.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for Grammar (AND)
static void grammar_destroy(GrammaticaGrammar* grammar);
static char* grammar_render(const GrammaticaGrammar* grammar, bool full, bool wrap);
static GrammaticaGrammar* grammar_simplify(const GrammaticaGrammar* grammar);
static GrammaticaGrammar* grammar_copy(const GrammaticaGrammar* grammar);
static bool grammar_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier);

// Forward declarations for Or
static void or_destroy(GrammaticaGrammar* grammar);
static char* or_render(const GrammaticaGrammar* grammar, bool full, bool wrap);
static GrammaticaGrammar* or_simplify(const GrammaticaGrammar* grammar);
static GrammaticaGrammar* or_copy(const GrammaticaGrammar* grammar);
static bool or_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier);

static const GrammaticaGrammarVTable grammar_vtable = {
    .destroy = grammar_destroy,
    .render = grammar_render,
    .simplify = grammar_simplify,
    .copy = grammar_copy,
    .equals = grammar_equals,
};

static const GrammaticaGrammarVTable or_vtable = {
    .destroy = or_destroy,
    .render = or_render,
    .simplify = or_simplify,
    .copy = or_copy,
    .equals = or_equals,
};

GrammaticaGroupedGrammar* grammaticaGrammarCreate(
    const GrammaticaGrammarArray* subexprs,
    GrammaticaQuantifier quantifier
) {
    if (subexprs == NULL) {
        return NULL;
    }
    if (grammaticaValidateQuantifier(quantifier) != GRAMMATICA_OK) {
        return NULL;
    }
    GrammaticaGroupedGrammar* grammar = (GrammaticaGroupedGrammar*)malloc(sizeof(GrammaticaGroupedGrammar));
    if (grammar == NULL) {
        return NULL;
    }
    grammar->subexprs = grammaticaGrammarArrayCopy(subexprs);
    if (grammar->subexprs == NULL) {
        free(grammar);
        return NULL;
    }
    grammar->quantifier = quantifier;
    grammar->base.type = GRAMMATICA_TYPE_GRAMMAR;
    grammar->base.vtable = &grammar_vtable;
    atomic_init(&grammar->base.refCount, 1);
    return grammar;
}

GrammaticaGroupedGrammar* grammaticaOrCreate(
    const GrammaticaGrammarArray* subexprs,
    GrammaticaQuantifier quantifier
) {
    if (subexprs == NULL) {
        return NULL;
    }
    if (grammaticaValidateQuantifier(quantifier) != GRAMMATICA_OK) {
        return NULL;
    }
    GrammaticaGroupedGrammar* grammar = (GrammaticaGroupedGrammar*)malloc(sizeof(GrammaticaGroupedGrammar));
    if (grammar == NULL) {
        return NULL;
    }
    grammar->subexprs = grammaticaGrammarArrayCopy(subexprs);
    if (grammar->subexprs == NULL) {
        free(grammar);
        return NULL;
    }
    grammar->quantifier = quantifier;
    grammar->base.type = GRAMMATICA_TYPE_OR;
    grammar->base.vtable = &or_vtable;
    atomic_init(&grammar->base.refCount, 1);
    return grammar;
}

const GrammaticaGrammarArray* grammaticaGroupedGetSubexprs(const GrammaticaGroupedGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    return grammar->subexprs;
}

GrammaticaQuantifier grammaticaGroupedGetQuantifier(const GrammaticaGroupedGrammar* grammar) {
    if (grammar == NULL) {
        return (GrammaticaQuantifier){1, 1};
    }
    return grammar->quantifier;
}

static void grouped_destroy_common(GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return;
    }
    GrammaticaGroupedGrammar* grouped = (GrammaticaGroupedGrammar*)grammar;
    if (grouped->subexprs != NULL) {
        grammaticaGrammarArrayDestroy(grouped->subexprs);
    }
    free(grouped);
}

static void grammar_destroy(GrammaticaGrammar* grammar) {
    grouped_destroy_common(grammar);
}

static void or_destroy(GrammaticaGrammar* grammar) {
    grouped_destroy_common(grammar);
}

static bool needs_wrapped_grammar(const GrammaticaGroupedGrammar* grammar) {
    if (grammar->subexprs->count < 1) {
        return false;
    }
    if (grammar->subexprs->count == 1) {
        if (grammar->quantifier.min == 1 && grammar->quantifier.max == 1) {
            return false;
        }
        // Check if the single subexpr needs wrapping
        GrammaticaGrammar* subexpr = grammar->subexprs->items[0];
        if (subexpr->type != GRAMMATICA_TYPE_GRAMMAR && 
            subexpr->type != GRAMMATICA_TYPE_OR) {
            return false;
        }
    }
    return grammar->quantifier.min != 1 || grammar->quantifier.max != 1;
}

static bool needs_wrapped_or(const GrammaticaGroupedGrammar* grammar) {
    if (grammar->subexprs->count < 1) {
        return false;
    }
    if (grammar->subexprs->count == 1) {
        if (grammar->quantifier.min == 1 && grammar->quantifier.max == 1) {
            return false;
        }
        // Check if the single subexpr needs wrapping
        GrammaticaGrammar* subexpr = grammar->subexprs->items[0];
        if (subexpr->type != GRAMMATICA_TYPE_GRAMMAR && 
            subexpr->type != GRAMMATICA_TYPE_OR) {
            return false;
        }
    }
    return true;
}

static char* grouped_render_common(const GrammaticaGrammar* grammar, bool full, bool wrap, const char* separator, bool is_or) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaGroupedGrammar* grouped = (const GrammaticaGroupedGrammar*)grammar;
    if (grouped->subexprs->count < 1) {
        return NULL;
    }
    bool needs_wrap = is_or ? needs_wrapped_or(grouped) : needs_wrapped_grammar(grouped);
    char* rendered_quantifier = grammaticaRenderQuantifier(grouped->quantifier);
    GrammaticaStringBuilder* sb = grammaticaStringBuilderCreate(0);
    if (sb == NULL) {
        if (rendered_quantifier != NULL) {
            free(rendered_quantifier);
        }
        return NULL;
    }
    bool found = false;
    for (size_t i = 0; i < grouped->subexprs->count; i++) {
        char* rendered = grammaticaGrammarRender(grouped->subexprs->items[i], false, true);
        if (rendered != NULL) {
            if (found) {
                grammaticaStringBuilderAppend(sb, separator);
            }
            grammaticaStringBuilderAppend(sb, rendered);
            free(rendered);
            found = true;
        }
    }
    if (!found) {
        grammaticaStringBuilderDestroy(sb);
        if (rendered_quantifier != NULL) {
            free(rendered_quantifier);
        }
        return NULL;
    }
    char* expr = grammaticaStringBuilderExtract(sb);
    grammaticaStringBuilderDestroy(sb);
    if (expr == NULL) {
        if (rendered_quantifier != NULL) {
            free(rendered_quantifier);
        }
        return NULL;
    }
    // Wrap if needed
    if (needs_wrap && (wrap || (rendered_quantifier != NULL))) {
        size_t len = strlen(expr);
        char* wrapped = (char*)malloc(len + 3);  // ( + expr + )
        if (wrapped != NULL) {
            snprintf(wrapped, len + 3, "(%s)", expr);
            free(expr);
            expr = wrapped;
        }
    }
    // Append quantifier if present
    if (rendered_quantifier != NULL) {
        size_t len = strlen(expr) + strlen(rendered_quantifier) + 1;
        char* with_quantifier = (char*)malloc(len);
        if (with_quantifier != NULL) {
            snprintf(with_quantifier, len, "%s%s", expr, rendered_quantifier);
            free(expr);
            expr = with_quantifier;
        }
        free(rendered_quantifier);
    }
    return expr;
}

static char* grammar_render(const GrammaticaGrammar* grammar, bool full, bool wrap) {
    return grouped_render_common(grammar, full, wrap, " ", false);
}

static char* or_render(const GrammaticaGrammar* grammar, bool full, bool wrap) {
    return grouped_render_common(grammar, full, wrap, " | ", true);
}

// Simplified version of simplify - full implementation would be more complex
static GrammaticaGrammar* grammar_simplify(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaGroupedGrammar* grouped = (const GrammaticaGroupedGrammar*)grammar;
    // Create a new array with simplified subexprs
    GrammaticaGrammarArray* simplified_array = grammaticaGrammarArrayCreate(0);
    if (simplified_array == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < grouped->subexprs->count; i++) {
        GrammaticaGrammar* simplified = grammaticaGrammarSimplify(grouped->subexprs->items[i]);
        if (simplified != NULL) {
            grammaticaGrammarArrayAppend(simplified_array, simplified);
            grammaticaGrammarUnref(simplified);
        }
    }
    if (simplified_array->count == 0) {
        grammaticaGrammarArrayDestroy(simplified_array);
        return NULL;
    }
    // Merge adjacent strings
    grammaticaMergeAdjacentStrings(simplified_array);
    // If only one subexpr and default quantifier, unwrap
    if (simplified_array->count == 1 && 
        grouped->quantifier.min == 1 && 
        grouped->quantifier.max == 1) {
        GrammaticaGrammar* result = grammaticaGrammarRef(simplified_array->items[0]);
        grammaticaGrammarArrayDestroy(simplified_array);
        return result;
    }
    GrammaticaGroupedGrammar* result = grammaticaGrammarCreate(simplified_array, grouped->quantifier);
    grammaticaGrammarArrayDestroy(simplified_array);
    return (GrammaticaGrammar*)result;
}

static GrammaticaGrammar* or_simplify(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaGroupedGrammar* grouped = (const GrammaticaGroupedGrammar*)grammar;
    // Create a new array with simplified subexprs
    GrammaticaGrammarArray* simplified_array = grammaticaGrammarArrayCreate(0);
    if (simplified_array == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < grouped->subexprs->count; i++) {
        GrammaticaGrammar* simplified = grammaticaGrammarSimplify(grouped->subexprs->items[i]);
        if (simplified != NULL) {
            // Check for duplicates
            bool duplicate = false;
            for (size_t j = 0; j < simplified_array->count; j++) {
                if (grammaticaGrammarEquals(simplified, simplified_array->items[j], true)) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                grammaticaGrammarArrayAppend(simplified_array, simplified);
            }
            grammaticaGrammarUnref(simplified);
        }
    }
    if (simplified_array->count == 0) {
        grammaticaGrammarArrayDestroy(simplified_array);
        return NULL;
    }
    // If only one subexpr and default quantifier, unwrap to Grammar
    if (simplified_array->count == 1 && 
        grouped->quantifier.min == 1 && 
        grouped->quantifier.max == 1) {
        GrammaticaGrammar* result = grammaticaGrammarRef(simplified_array->items[0]);
        grammaticaGrammarArrayDestroy(simplified_array);
        return result;
    }
    GrammaticaGroupedGrammar* result = grammaticaOrCreate(simplified_array, grouped->quantifier);
    grammaticaGrammarArrayDestroy(simplified_array);
    return (GrammaticaGrammar*)result;
}

static GrammaticaGrammar* grouped_copy_common(const GrammaticaGrammar* grammar, bool is_or) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaGroupedGrammar* grouped = (const GrammaticaGroupedGrammar*)grammar;
    if (is_or) {
        return (GrammaticaGrammar*)grammaticaOrCreate(grouped->subexprs, grouped->quantifier);
    } else {
        return (GrammaticaGrammar*)grammaticaGrammarCreate(grouped->subexprs, grouped->quantifier);
    }
}

static GrammaticaGrammar* grammar_copy(const GrammaticaGrammar* grammar) {
    return grouped_copy_common(grammar, false);
}

static GrammaticaGrammar* or_copy(const GrammaticaGrammar* grammar) {
    return grouped_copy_common(grammar, true);
}

static bool grouped_equals_common(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier) {
    if (grammar == other) {
        return true;
    }
    if (grammar == NULL || other == NULL) {
        return false;
    }
    if (grammar->type != other->type) {
        return false;
    }
    const GrammaticaGroupedGrammar* g1 = (const GrammaticaGroupedGrammar*)grammar;
    const GrammaticaGroupedGrammar* g2 = (const GrammaticaGroupedGrammar*)other;
    if (check_quantifier) {
        if (g1->quantifier.min != g2->quantifier.min || 
            g1->quantifier.max != g2->quantifier.max) {
            return false;
        }
    }
    if (g1->subexprs->count != g2->subexprs->count) {
        return false;
    }
    for (size_t i = 0; i < g1->subexprs->count; i++) {
        if (!grammaticaGrammarEquals(g1->subexprs->items[i], g2->subexprs->items[i], check_quantifier)) {
            return false;
        }
    }
    return true;
}

static bool grammar_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier) {
    return grouped_equals_common(grammar, other, check_quantifier);
}

static bool or_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier) {
    return grouped_equals_common(grammar, other, check_quantifier);
}
