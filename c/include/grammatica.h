#ifndef GRAMMATICA_H
#define GRAMMATICA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct GrammaticaContext_t* GrammaticaContextHandle_t;

typedef void (*GrammaticaErrorHandler)(const char* message, void* userdata);

typedef void (*GrammaticaNoticeHandler)(const char* message, void* userdata);

typedef enum {
	GRAMMATICA_ERROR_NONE = 0,          /* No error */
	GRAMMATICA_ERROR_INVALID_CONTEXT,   /* Invalid or NULL context */
	GRAMMATICA_ERROR_INVALID_PARAMETER, /* Invalid function parameter */
	GRAMMATICA_ERROR_OUT_OF_MEMORY,     /* Memory allocation failed */
	GRAMMATICA_ERROR_INVALID_GRAMMAR,   /* Invalid grammar structure */
	GRAMMATICA_ERROR_SIMPLIFICATION,    /* Error during simplification */
	GRAMMATICA_ERROR_RENDER,            /* Error during rendering */
	GRAMMATICA_ERROR_COPY,              /* Error during copy operation */
	GRAMMATICA_ERROR_UNKNOWN            /* Unknown/unspecified error */
} GrammaticaErrorCode;

GrammaticaContextHandle_t grammatica_init(void);
void grammatica_finish(GrammaticaContextHandle_t ctx);
void grammatica_set_error_handler(GrammaticaContextHandle_t ctx, GrammaticaErrorHandler handler, void* userdata);
void grammatica_set_notice_handler(GrammaticaContextHandle_t ctx, GrammaticaNoticeHandler handler, void* userdata);

const char* grammatica_get_last_error(GrammaticaContextHandle_t ctx);
GrammaticaError_t grammatica_get_last_error_code(GrammaticaContextHandle_t ctx);
const char* grammatica_error_code_to_string(GrammaticaError_t code);
void grammatica_clear_error(GrammaticaContextHandle_t ctx);

typedef struct Grammar_t Grammar;
typedef struct CharRange_t CharRange;
typedef struct String_t String;
typedef struct DerivationRule_t DerivationRule;
typedef struct And_t And;
typedef struct Or_t Or;

typedef enum { GRAMMAR_TYPE_CHAR_RANGE, GRAMMAR_TYPE_STRING, GRAMMAR_TYPE_DERIVATION_RULE, GRAMMAR_TYPE_AND, GRAMMAR_TYPE_OR } GrammarType;

typedef struct {
	int lower;
	int upper; /* -1 for infinity */
} Quantifier;

typedef struct {
	char start;
	char end;
} CharRangePair;

CharRange* grammatica_char_range_create(GrammaticaContextHandle_t ctx, const CharRangePair* ranges, size_t n, bool negate);
CharRange* grammatica_char_range_from_chars(GrammaticaContextHandle_t ctx, const char* chars, size_t n, bool negate);
CharRange* grammatica_char_range_from_ords(GrammaticaContextHandle_t ctx, const int* ords, size_t n, bool negate);
void grammatica_char_range_destroy(CharRange* range);
char* grammatica_char_range_render(GrammaticaContextHandle_t ctx, const CharRange* range, bool full, bool wrap);
Grammar* grammatica_char_range_simplify(GrammaticaContextHandle_t ctx, const CharRange* range);
char* grammatica_char_range_as_string(GrammaticaContextHandle_t ctx, const CharRange* range);
bool grammatica_char_range_equals(GrammaticaContextHandle_t ctx, const CharRange* a, const CharRange* b);
CharRange* grammatica_char_range_copy(GrammaticaContextHandle_t ctx, const CharRange* range);
size_t grammatica_char_range_get_ranges_n(GrammaticaContextHandle_t ctx, const CharRange* range);
int grammatica_char_range_get_ranges(GrammaticaContextHandle_t ctx, const CharRange* range, CharRangePair* out_ranges, size_t max_ranges);
bool grammatica_char_range_get_negate(GrammaticaContextHandle_t ctx, const CharRange* range);

String* grammatica_string_create(GrammaticaContextHandle_t ctx, const unsigned char* g);
void grammatica_string_destroy(String* g);
unsigned char* grammatica_string_render(GrammaticaContextHandle_t ctx, const String* g);
Grammar* grammatica_string_simplify(GrammaticaContextHandle_t ctx, const String* g);
unsigned char* grammatica_string_as_string(GrammaticaContextHandle_t ctx, const String* g);
bool grammatica_string_equals(GrammaticaContextHandle_t ctx, const String* a, const String* b);
String* grammatica_string_copy(GrammaticaContextHandle_t ctx, const String* g);

DerivationRule* grammatica_derivation_rule_create(GrammaticaContextHandle_t ctx, const char* symbol, Grammar* value);
void grammatica_derivation_rule_destroy(DerivationRule* rule);
char* grammatica_derivation_rule_render(GrammaticaContextHandle_t ctx, const DerivationRule* rule, bool full, bool wrap);
Grammar* grammatica_derivation_rule_simplify(GrammaticaContextHandle_t ctx, const DerivationRule* rule);
char* grammatica_derivation_rule_as_string(GrammaticaContextHandle_t ctx, const DerivationRule* rule);
bool grammatica_derivation_rule_equals(GrammaticaContextHandle_t ctx, const DerivationRule* a, const DerivationRule* b);
DerivationRule* grammatica_derivation_rule_copy(GrammaticaContextHandle_t ctx, const DerivationRule* rule);
const char* grammatica_derivation_rule_get_symbol(GrammaticaContextHandle_t ctx, const DerivationRule* rule);
const Grammar* grammatica_derivation_rule_get_value(GrammaticaContextHandle_t ctx, const DerivationRule* rule);

And* grammatica_and_create(GrammaticaContextHandle_t ctx, Grammar** subexprs, size_t subexprs_n, Quantifier quantifier);
void grammatica_and_destroy(And* and_expr);
char* grammatica_and_render(GrammaticaContextHandle_t ctx, const And* and_expr, bool full, bool wrap);
Grammar* grammatica_and_simplify(GrammaticaContextHandle_t ctx, const And* and_expr);
char* grammatica_and_as_string(GrammaticaContextHandle_t ctx, const And* and_expr);
bool grammatica_and_equals(GrammaticaContextHandle_t ctx, const And* a, const And* b, bool check_quantifier);
And* grammatica_and_copy(GrammaticaContextHandle_t ctx, const And* and_expr);
size_t grammatica_and_get_subexprs_n(GrammaticaContextHandle_t ctx, const And* and_expr);
int grammatica_and_get_subexprs(GrammaticaContextHandle_t ctx, const And* and_expr, Grammar** out_subexprs, size_t max_subexprs);
Quantifier grammatica_and_get_quantifier(GrammaticaContextHandle_t ctx, const And* and_expr);

Or* grammatica_or_create(GrammaticaContextHandle_t ctx, Grammar** subexprs, size_t subexprs_n, Quantifier quantifier);
void grammatica_or_destroy(Or* or_expr);
char* grammatica_or_render(GrammaticaContextHandle_t ctx, const Or* or_expr, bool full, bool wrap);
Grammar* grammatica_or_simplify(GrammaticaContextHandle_t ctx, const Or* or_expr);
char* grammatica_or_as_string(GrammaticaContextHandle_t ctx, const Or* or_expr);
bool grammatica_or_equals(GrammaticaContextHandle_t ctx, const Or* a, const Or* b, bool check_quantifier);
Or* grammatica_or_copy(GrammaticaContextHandle_t ctx, const Or* or_expr);
size_t grammatica_or_get_subexprs_n(GrammaticaContextHandle_t ctx, const Or* or_expr);
int grammatica_or_get_subexprs(GrammaticaContextHandle_t ctx, const Or* or_expr, Grammar** out_subexprs, size_t max_subexprs);
Quantifier grammatica_or_get_quantifier(GrammaticaContextHandle_t ctx, const Or* or_expr);

GrammarType grammatica_grammar_get_type(GrammaticaContextHandle_t ctx, const Grammar* grammar);
void grammatica_grammar_destroy(Grammar* grammar);
char* grammatica_grammar_render(GrammaticaContextHandle_t ctx, const Grammar* grammar, bool full, bool wrap);
Grammar* grammatica_grammar_simplify(GrammaticaContextHandle_t ctx, const Grammar* grammar);
char* grammatica_grammar_as_string(GrammaticaContextHandle_t ctx, const Grammar* grammar);
bool grammatica_grammar_equals(GrammaticaContextHandle_t ctx, const Grammar* a, const Grammar* b);
Grammar* grammatica_grammar_copy(GrammaticaContextHandle_t ctx, const Grammar* grammar);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_H */
