#define _GNU_SOURCE
#include "base.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

GrammaticaGrammar* grammaticaGrammarRef(GrammaticaGrammar* grammar) {
    if (grammar != NULL) {
        atomic_fetch_add(&grammar->refCount, 1);
    }
    return grammar;
}

void grammaticaGrammarUnref(GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return;
    }
    int old_count = atomic_fetch_sub(&grammar->refCount, 1);
    if (old_count == 1) {
        // Last reference, destroy the grammar
        if (grammar->vtable != NULL && grammar->vtable->destroy != NULL) {
            grammar->vtable->destroy(grammar);
        }
    }
}

char* grammaticaGrammarRender(const GrammaticaGrammar* grammar, bool full, bool wrap) {
    if (grammar == NULL) {
        return NULL;
    }
    if (grammar->vtable == NULL || grammar->vtable->render == NULL) {
        return NULL;
    }
    return grammar->vtable->render(grammar, full, wrap);
}

GrammaticaGrammar* grammaticaGrammarSimplify(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    if (grammar->vtable == NULL || grammar->vtable->simplify == NULL) {
        return NULL;
    }
    return grammar->vtable->simplify(grammar);
}

GrammaticaGrammar* grammaticaGrammarCopy(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    if (grammar->vtable == NULL || grammar->vtable->copy == NULL) {
        return NULL;
    }
    return grammar->vtable->copy(grammar);
}

bool grammaticaGrammarEquals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier) {
    if (grammar == other) {
        return true;
    }
    if (grammar == NULL || other == NULL) {
        return false;
    }
    if (grammar->type != other->type) {
        return false;
    }
    if (grammar->vtable == NULL || grammar->vtable->equals == NULL) {
        return false;
    }
    return grammar->vtable->equals(grammar, other, check_quantifier);
}

GrammaticaError_t grammaticaValidateQuantifier(GrammaticaQuantifier quantifier) {
    // Check if max is valid (0 means infinity, otherwise must be >= 1)
    if (quantifier.max != 0 && quantifier.max < 1) {
        return GRAMMATICA_ERROR_INVALID_ARGUMENT;
    }
    // Check if min > max (when max is not infinity)
    if (quantifier.max != 0 && quantifier.min > quantifier.max) {
        return GRAMMATICA_ERROR_INVALID_ARGUMENT;
    }
    return GRAMMATICA_OK;
}

char* grammaticaRenderQuantifier(GrammaticaQuantifier quantifier) {
    uint32_t min = quantifier.min;
    uint32_t max = quantifier.max;
    // Default quantifier (1, 1) doesn't need to be rendered
    if (min == 1 && max == 1) {
        return NULL;
    }
    // Common patterns
    if (min == 0 && max == 0) {
        return strdup("*");
    }
    if (min == 0 && max == 1) {
        return strdup("?");
    }
    if (min == 1 && max == 0) {
        return strdup("+");
    }
    // Range patterns
    char* result = (char*)malloc(64);  // Should be enough for any quantifier
    if (result == NULL) {
        return NULL;
    }
    if (min == max) {
        snprintf(result, 64, "{%u}", min);
    } else if (max == 0) {
        snprintf(result, 64, "{%u,}", min);
    } else {
        snprintf(result, 64, "{%u,%u}", min, max);
    }
    return result;
}

GrammaticaGrammarArray* grammaticaGrammarArrayCreate(size_t initial_capacity) {
    GrammaticaGrammarArray* array = (GrammaticaGrammarArray*)malloc(sizeof(GrammaticaGrammarArray));
    if (array == NULL) {
        return NULL;
    }
    if (initial_capacity == 0) {
        initial_capacity = 8;
    }
    array->items = (GrammaticaGrammar**)malloc(initial_capacity * sizeof(GrammaticaGrammar*));
    if (array->items == NULL) {
        free(array);
        return NULL;
    }
    array->count = 0;
    array->capacity = initial_capacity;
    return array;
}

void grammaticaGrammarArrayDestroy(GrammaticaGrammarArray* array) {
    if (array == NULL) {
        return;
    }
    if (array->items != NULL) {
        for (size_t i = 0; i < array->count; i++) {
            grammaticaGrammarUnref(array->items[i]);
        }
        free(array->items);
    }
    free(array);
}

GrammaticaError_t grammaticaGrammarArrayAppend(GrammaticaGrammarArray* array, GrammaticaGrammar* grammar) {
    if (array == NULL || grammar == NULL) {
        return GRAMMATICA_ERROR_NULL_POINTER;
    }
    if (array->count >= array->capacity) {
        size_t new_capacity = array->capacity * 2;
        GrammaticaGrammar** new_items = (GrammaticaGrammar**)realloc(array->items, new_capacity * sizeof(GrammaticaGrammar*));
        if (new_items == NULL) {
            return GRAMMATICA_ERROR_OUT_OF_MEMORY;
        }
        array->items = new_items;
        array->capacity = new_capacity;
    }
    array->items[array->count] = grammaticaGrammarRef(grammar);
    array->count++;
    return GRAMMATICA_OK;
}

GrammaticaGrammarArray* grammaticaGrammarArrayCopy(const GrammaticaGrammarArray* array) {
    if (array == NULL) {
        return NULL;
    }
    GrammaticaGrammarArray* copy = grammaticaGrammarArrayCreate(array->count);
    if (copy == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < array->count; i++) {
        if (grammaticaGrammarArrayAppend(copy, array->items[i]) != GRAMMATICA_OK) {
            grammaticaGrammarArrayDestroy(copy);
            return NULL;
        }
    }
    return copy;
}
