#define _GNU_SOURCE
#include "grammatica/grammar/string.h"
#include "grammatica/constants.h"
#include "grammatica/utils.h"
#include <stdlib.h>
#include <string.h>

// Forward declarations
static void string_destroy(GrammaticaGrammar* grammar);
static char* string_render(const GrammaticaGrammar* grammar, bool full, bool wrap);
static GrammaticaGrammar* string_simplify(const GrammaticaGrammar* grammar);
static GrammaticaGrammar* string_copy(const GrammaticaGrammar* grammar);
static bool string_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier);

static const GrammaticaGrammarVTable string_vtable = {
    .destroy = string_destroy,
    .render = string_render,
    .simplify = string_simplify,
    .copy = string_copy,
    .equals = string_equals,
};

GrammaticaString* grammaticaStringCreate(const char* value) {
    if (value == NULL) {
        return NULL;
    }
    GrammaticaString* grammar = (GrammaticaString*)malloc(sizeof(GrammaticaString));
    if (grammar == NULL) {
        return NULL;
    }
    grammar->value = strdup(value);
    if (grammar->value == NULL) {
        free(grammar);
        return NULL;
    }
    grammar->base.type = GRAMMATICA_TYPE_STRING;
    grammar->base.vtable = &string_vtable;
    atomic_init(&grammar->base.refCount, 1);
    return grammar;
}

const char* grammaticaStringGetValue(const GrammaticaString* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    return grammar->value;
}

static void string_destroy(GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return;
    }
    GrammaticaString* string_grammar = (GrammaticaString*)grammar;
    if (string_grammar->value != NULL) {
        free(string_grammar->value);
    }
    free(string_grammar);
}

static char* string_render(const GrammaticaGrammar* grammar, bool full, bool wrap) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaString* string_grammar = (const GrammaticaString*)grammar;
    if (string_grammar->value == NULL || string_grammar->value[0] == '\0') {
        return NULL;
    }
    GrammaticaStringBuilder* sb = grammaticaStringBuilderCreate(0);
    if (sb == NULL) {
        return NULL;
    }
    grammaticaStringBuilderAppend_char(sb, '"');
    const char* p = string_grammar->value;
    while (*p != '\0') {
        uint32_t ch = (uint32_t)(unsigned char)*p;
        // Check if always safe
        if (grammaticaIsAlwaysSafeChar(ch)) {
            grammaticaStringBuilderAppend_char(sb, (char)ch);
        }
        // Check for standard escape sequences
        else {
            char escape_buf[3];
            if (grammaticaGetCharEscape(ch, escape_buf)) {
                grammaticaStringBuilderAppend(sb, escape_buf);
            }
            // Check for string literal escape characters
            else if (grammaticaIsStringLiteralEscapeChar(ch)) {
                grammaticaStringBuilderAppend_char(sb, '\\');
                grammaticaStringBuilderAppend_char(sb, (char)ch);
            }
            // Otherwise use hex escape
            else {
                char hex_buf[16];
                if (Grammatica_charToHex(ch, hex_buf, sizeof(hex_buf)) == GRAMMATICA_OK) {
                    grammaticaStringBuilderAppend(sb, hex_buf);
                }
            }
        }
        p++;
    }
    grammaticaStringBuilderAppend_char(sb, '"');
    char* result = grammaticaStringBuilderExtract(sb);
    grammaticaStringBuilderDestroy(sb);
    return result;
}

static GrammaticaGrammar* string_simplify(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaString* string_grammar = (const GrammaticaString*)grammar;
    if (string_grammar->value == NULL || string_grammar->value[0] == '\0') {
        return NULL;
    }
    return (GrammaticaGrammar*)grammaticaStringCreate(string_grammar->value);
}

static GrammaticaGrammar* string_copy(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaString* string_grammar = (const GrammaticaString*)grammar;
    return (GrammaticaGrammar*)grammaticaStringCreate(string_grammar->value);
}

static bool string_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier) {
    if (grammar == other) {
        return true;
    }
    if (grammar == NULL || other == NULL) {
        return false;
    }
    if (grammar->type != other->type) {
        return false;
    }
    const GrammaticaString* g1 = (const GrammaticaString*)grammar;
    const GrammaticaString* g2 = (const GrammaticaString*)other;
    if (g1->value == NULL && g2->value == NULL) {
        return true;
    }
    if (g1->value == NULL || g2->value == NULL) {
        return false;
    }
    return strcmp(g1->value, g2->value) == 0;
}

GrammaticaError_t grammaticaMergeAdjacentStrings(GrammaticaGrammarArray* array) {
    if (array == NULL) {
        return GRAMMATICA_ERROR_NULL_POINTER;
    }
    if (array->count < 2) {
        return GRAMMATICA_OK;  // Nothing to merge
    }
    size_t write_pos = 0;
    size_t read_pos = 0;
    while (read_pos < array->count) {
        // Find the start of a sequence of strings
        if (array->items[read_pos]->type == GRAMMATICA_TYPE_STRING) {
            // Collect all adjacent strings
            GrammaticaStringBuilder* sb = grammaticaStringBuilderCreate(0);
            if (sb == NULL) {
                return GRAMMATICA_ERROR_OUT_OF_MEMORY;
            }
            while (read_pos < array->count && array->items[read_pos]->type == GRAMMATICA_TYPE_STRING) {
                const GrammaticaString* str = (const GrammaticaString*)array->items[read_pos];
                grammaticaStringBuilderAppend(sb, str->value);
                // Unref the old string (except if it's the first one, we'll replace it)
                if (read_pos != write_pos) {
                    grammaticaGrammarUnref(array->items[read_pos]);
                }
                read_pos++;
            }
            // Create a new merged string
            char* merged_value = grammaticaStringBuilderExtract(sb);
            grammaticaStringBuilderDestroy(sb);
            if (merged_value == NULL) {
                return GRAMMATICA_ERROR_OUT_OF_MEMORY;
            }
            GrammaticaString* merged_string = grammaticaStringCreate(merged_value);
            free(merged_value);
            if (merged_string == NULL) {
                return GRAMMATICA_ERROR_OUT_OF_MEMORY;
            }
            // Unref the old item at write_pos and replace with merged
            grammaticaGrammarUnref(array->items[write_pos]);
            array->items[write_pos] = (GrammaticaGrammar*)merged_string;
            write_pos++;
        } else {
            // Not a string, just copy it
            if (write_pos != read_pos) {
                array->items[write_pos] = array->items[read_pos];
            }
            write_pos++;
            read_pos++;
        }
    }
    // Update the count
    array->count = write_pos;
    return GRAMMATICA_OK;
}
