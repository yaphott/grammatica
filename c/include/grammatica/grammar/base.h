/**
 * @file grammar/base.h
 * @brief Base grammar structures and functions
 */

#ifndef GRAMMATICA_GRAMMAR_BASE_H
#define GRAMMATICA_GRAMMAR_BASE_H

#include "../common.h"
#include "../string_builder.h"

// Atomic operations - use C atomics in C, C++ atomics in C++
#ifdef __cplusplus
#include <atomic>
typedef std::atomic<int> atomic_int_t;
#else
#include <stdatomic.h>
typedef atomic_int atomic_int_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Forward declarations
 */
typedef struct grammatica_grammar_s GrammaticaGrammar;

/**
 * @brief Grammar type enumeration
 */
typedef enum {
    GRAMMATICA_TYPE_STRING,
    GRAMMATICA_TYPE_CHAR_RANGE,
    GRAMMATICA_TYPE_GRAMMAR,
    GRAMMATICA_TYPE_OR,
    GRAMMATICA_TYPE_DERIVATION_RULE,
} GrammaticaGrammarType;

/**
 * @brief Quantifier structure (min, max)
 * max == 0 means infinity
 */
typedef struct {
    uint32_t min;
    uint32_t max;  // 0 means infinity
} GrammaticaQuantifier;

/**
 * @brief Grammar virtual table for polymorphism
 */
typedef struct {
    void (*destroy)(GrammaticaGrammar* grammar);
    char* (*render)(const GrammaticaGrammar* grammar, bool full, bool wrap);
    GrammaticaGrammar* (*simplify)(const GrammaticaGrammar* grammar);
    GrammaticaGrammar* (*copy)(const GrammaticaGrammar* grammar);
    bool (*equals)(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier);
} GrammaticaGrammarVTable;

/**
 * @brief Base grammar structure
 */
struct grammatica_grammar_s {
    GrammaticaGrammarType type;
    const GrammaticaGrammarVTable* vtable;
    atomic_int_t refCount;
};

/**
 * @brief Increment reference count of a grammar
 * @param grammar Grammar to increment
 * @return The same grammar pointer
 */
GrammaticaGrammar* grammaticaGrammarRef(GrammaticaGrammar* grammar);

/**
 * @brief Decrement reference count and destroy if zero
 * @param grammar Grammar to decrement
 */
void grammaticaGrammarUnref(GrammaticaGrammar* grammar);

/**
 * @brief Render the grammar as a string
 * @param grammar Grammar to render
 * @param full Render the full expression
 * @param wrap Wrap the expression in parentheses
 * @return Rendered string (caller must free), or NULL if empty
 */
char* grammaticaGrammarRender(const GrammaticaGrammar* grammar, bool full, bool wrap);

/**
 * @brief Simplify the grammar
 * @param grammar Grammar to simplify
 * @return Simplified grammar (new reference), or NULL if empty
 */
GrammaticaGrammar* grammaticaGrammarSimplify(const GrammaticaGrammar* grammar);

/**
 * @brief Copy the grammar
 * @param grammar Grammar to copy
 * @return Copy of the grammar (new reference)
 */
GrammaticaGrammar* grammaticaGrammarCopy(const GrammaticaGrammar* grammar);

/**
 * @brief Check if two grammars are equal
 * @param grammar First grammar
 * @param other Second grammar
 * @param check_quantifier Include quantifier in comparison
 * @return true if equal, false otherwise
 */
bool grammaticaGrammarEquals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier);

/**
 * @brief Validate a quantifier
 * @param quantifier Quantifier to validate
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError grammaticaValidateQuantifier(GrammaticaQuantifier quantifier);

/**
 * @brief Render a quantifier as a string
 * @param quantifier Quantifier to render
 * @return Rendered string (caller must free), or NULL if not applicable
 */
char* grammaticaRenderQuantifier(GrammaticaQuantifier quantifier);

/**
 * @brief Array of grammars
 */
typedef struct {
    GrammaticaGrammar** items;
    size_t count;
    size_t capacity;
} GrammaticaGrammarArray;

/**
 * @brief Create a new grammar array
 * @param initial_capacity Initial capacity
 * @return Pointer to the new array, or NULL on error
 */
GrammaticaGrammarArray* grammaticaGrammarArrayCreate(size_t initial_capacity);

/**
 * @brief Destroy a grammar array and unref all grammars
 * @param array Array to destroy
 */
void grammaticaGrammarArrayDestroy(GrammaticaGrammarArray* array);

/**
 * @brief Append a grammar to the array
 * @param array Array
 * @param grammar Grammar to append (will be ref'd)
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError grammaticaGrammarArrayAppend(GrammaticaGrammarArray* array, GrammaticaGrammar* grammar);

/**
 * @brief Copy a grammar array
 * @param array Array to copy
 * @return Copy of the array, or NULL on error
 */
GrammaticaGrammarArray* grammaticaGrammarArrayCopy(const GrammaticaGrammarArray* array);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_GRAMMAR_BASE_H */
