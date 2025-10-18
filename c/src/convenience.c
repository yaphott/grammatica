/**
 * Convenience helper functions for the Grammatica library.
 * 
 * These functions provide simpler, more user-friendly ways to create
 * common grammar patterns without requiring verbose boilerplate code.
 */

#include "grammatica.h"
#include "grammatica_internal.h"
#include <stdlib.h>
#include <string.h>

/* ========================================================================
 * Literal String Helper
 * ======================================================================== */

/**
 * Create a string grammar from a C string literal.
 * 
 * This is a convenience wrapper around grammatica_string_create() that
 * wraps the result in a Grammar object.
 * 
 * @param ctx The grammatica context
 * @param str The string value (will be copied)
 * @return Grammar object containing the string, or NULL on error
 * 
 * Example:
 *   Grammar* hello = grammatica_literal(ctx, "hello");
 */
Grammar* grammatica_literal(GrammaticaContextHandle_t ctx, const char* str) {
	VALIDATE_PARAM_RET_NULL(ctx, str, "String parameter cannot be NULL");
	
	String* string_obj = grammatica_string_create(ctx, str);
	if (!string_obj) {
		return NULL;
	}
	
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_string_destroy(ctx, string_obj);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for literal");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_STRING;
	result->data = string_obj;
	return result;
}

/* ========================================================================
 * Character Class Helpers
 * ======================================================================== */

/**
 * Create a digit character class grammar [0-9].
 * 
 * @param ctx The grammatica context
 * @return Grammar object for [0-9], or NULL on error
 * 
 * Example:
 *   Grammar* digit = grammatica_digit(ctx);
 */
Grammar* grammatica_digit(GrammaticaContextHandle_t ctx) {
	VALIDATE_CONTEXT_RET_NULL(ctx);
	
	CharRangePair range = {48, 57}; /* '0' to '9' */
	CharRange* char_range = grammatica_char_range_create(ctx, &range, 1, false);
	if (!char_range) {
		return NULL;
	}
	
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_char_range_destroy(ctx, char_range);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for digit");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_CHAR_RANGE;
	result->data = char_range;
	return result;
}

/**
 * Create an alphabetic character class grammar [a-zA-Z].
 * 
 * @param ctx The grammatica context
 * @return Grammar object for [a-zA-Z], or NULL on error
 * 
 * Example:
 *   Grammar* alpha = grammatica_alpha(ctx);
 */
Grammar* grammatica_alpha(GrammaticaContextHandle_t ctx) {
	VALIDATE_CONTEXT_RET_NULL(ctx);
	
	CharRangePair ranges[2] = {
		{65, 90},   /* 'A' to 'Z' */
		{97, 122}   /* 'a' to 'z' */
	};
	CharRange* char_range = grammatica_char_range_create(ctx, ranges, 2, false);
	if (!char_range) {
		return NULL;
	}
	
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_char_range_destroy(ctx, char_range);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for alpha");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_CHAR_RANGE;
	result->data = char_range;
	return result;
}

/**
 * Create an alphanumeric character class grammar [0-9a-zA-Z].
 * 
 * @param ctx The grammatica context
 * @return Grammar object for [0-9a-zA-Z], or NULL on error
 * 
 * Example:
 *   Grammar* alnum = grammatica_alnum(ctx);
 */
Grammar* grammatica_alnum(GrammaticaContextHandle_t ctx) {
	VALIDATE_CONTEXT_RET_NULL(ctx);
	
	CharRangePair ranges[3] = {
		{48, 57},   /* '0' to '9' */
		{65, 90},   /* 'A' to 'Z' */
		{97, 122}   /* 'a' to 'z' */
	};
	CharRange* char_range = grammatica_char_range_create(ctx, ranges, 3, false);
	if (!char_range) {
		return NULL;
	}
	
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_char_range_destroy(ctx, char_range);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for alnum");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_CHAR_RANGE;
	result->data = char_range;
	return result;
}

/**
 * Create a whitespace character class grammar [ \t\n\r].
 * 
 * @param ctx The grammatica context
 * @return Grammar object for whitespace characters, or NULL on error
 * 
 * Example:
 *   Grammar* ws = grammatica_whitespace(ctx);
 */
Grammar* grammatica_whitespace(GrammaticaContextHandle_t ctx) {
	VALIDATE_CONTEXT_RET_NULL(ctx);
	
	CharRangePair ranges[4] = {
		{9, 9},     /* '\t' (tab) */
		{10, 10},   /* '\n' (newline) */
		{13, 13},   /* '\r' (carriage return) */
		{32, 32}    /* ' ' (space) */
	};
	CharRange* char_range = grammatica_char_range_create(ctx, ranges, 4, false);
	if (!char_range) {
		return NULL;
	}
	
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_char_range_destroy(ctx, char_range);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for whitespace");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_CHAR_RANGE;
	result->data = char_range;
	return result;
}

/* ========================================================================
 * Quantifier Helpers
 * ======================================================================== */

/**
 * Create an optional grammar (zero or one occurrence) by wrapping g in
 * an AND with quantifier {0,1} (equivalent to g?).
 * 
 * @param ctx The grammatica context
 * @param g The grammar to make optional (will be copied)
 * @return Grammar with optional quantifier, or NULL on error
 * 
 * Example:
 *   Grammar* optional_digit = grammatica_optional(ctx, digit_grammar);
 */
Grammar* grammatica_optional(GrammaticaContextHandle_t ctx, const Grammar* g) {
	VALIDATE_PARAM_RET_NULL(ctx, g, "Grammar parameter cannot be NULL");
	
	/* Copy the input grammar */
	Grammar* copy = grammatica_grammar_copy(ctx, g);
	if (!copy) {
		return NULL;
	}
	
	/* Create an AND with quantifier {0, 1} */
	Quantifier quant = {0, 1};
	And* and_expr = grammatica_and_create(ctx, &copy, 1, quant);
	if (!and_expr) {
		grammatica_grammar_destroy(ctx, copy);
		return NULL;
	}
	
	/* Wrap in Grammar */
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_and_destroy(ctx, and_expr);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for optional");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_AND;
	result->data = and_expr;
	return result;
}

/**
 * Create a zero-or-more grammar (Kleene star) by wrapping g in
 * an AND with quantifier {0,∞} (equivalent to g*).
 * 
 * @param ctx The grammatica context
 * @param g The grammar to repeat (will be copied)
 * @return Grammar with zero-or-more quantifier, or NULL on error
 * 
 * Example:
 *   Grammar* many_digits = grammatica_zero_or_more(ctx, digit_grammar);
 */
Grammar* grammatica_zero_or_more(GrammaticaContextHandle_t ctx, const Grammar* g) {
	VALIDATE_PARAM_RET_NULL(ctx, g, "Grammar parameter cannot be NULL");
	
	/* Copy the input grammar */
	Grammar* copy = grammatica_grammar_copy(ctx, g);
	if (!copy) {
		return NULL;
	}
	
	/* Create an AND with quantifier {0, ∞} */
	Quantifier quant = {0, -1};
	And* and_expr = grammatica_and_create(ctx, &copy, 1, quant);
	if (!and_expr) {
		grammatica_grammar_destroy(ctx, copy);
		return NULL;
	}
	
	/* Wrap in Grammar */
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_and_destroy(ctx, and_expr);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for zero_or_more");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_AND;
	result->data = and_expr;
	return result;
}

/**
 * Create a one-or-more grammar (plus) by wrapping g in
 * an AND with quantifier {1,∞} (equivalent to g+).
 * 
 * @param ctx The grammatica context
 * @param g The grammar to repeat (will be copied)
 * @return Grammar with one-or-more quantifier, or NULL on error
 * 
 * Example:
 *   Grammar* some_digits = grammatica_one_or_more(ctx, digit_grammar);
 */
Grammar* grammatica_one_or_more(GrammaticaContextHandle_t ctx, const Grammar* g) {
	VALIDATE_PARAM_RET_NULL(ctx, g, "Grammar parameter cannot be NULL");
	
	/* Copy the input grammar */
	Grammar* copy = grammatica_grammar_copy(ctx, g);
	if (!copy) {
		return NULL;
	}
	
	/* Create an AND with quantifier {1, ∞} */
	Quantifier quant = {1, -1};
	And* and_expr = grammatica_and_create(ctx, &copy, 1, quant);
	if (!and_expr) {
		grammatica_grammar_destroy(ctx, copy);
		return NULL;
	}
	
	/* Wrap in Grammar */
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_and_destroy(ctx, and_expr);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for one_or_more");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_AND;
	result->data = and_expr;
	return result;
}

/**
 * Create an exact-repetition grammar by wrapping g in
 * an AND with quantifier {n,n} (equivalent to g{n}).
 * 
 * @param ctx The grammatica context
 * @param g The grammar to repeat (will be copied)
 * @param n The exact number of repetitions (must be >= 0)
 * @return Grammar with exact repetition quantifier, or NULL on error
 * 
 * Example:
 *   Grammar* three_digits = grammatica_repeat(ctx, digit_grammar, 3);
 */
Grammar* grammatica_repeat(GrammaticaContextHandle_t ctx, const Grammar* g, int n) {
	VALIDATE_PARAM_RET_NULL(ctx, g, "Grammar parameter cannot be NULL");
	
	if (n < 0) {
		grammatica_report_error(ctx, "Repetition count must be non-negative");
		return NULL;
	}
	
	/* Copy the input grammar */
	Grammar* copy = grammatica_grammar_copy(ctx, g);
	if (!copy) {
		return NULL;
	}
	
	/* Create an AND with quantifier {n, n} */
	Quantifier quant = {n, n};
	And* and_expr = grammatica_and_create(ctx, &copy, 1, quant);
	if (!and_expr) {
		grammatica_grammar_destroy(ctx, copy);
		return NULL;
	}
	
	/* Wrap in Grammar */
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_and_destroy(ctx, and_expr);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for repeat");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_AND;
	result->data = and_expr;
	return result;
}

/* ========================================================================
 * Composition Helpers
 * ======================================================================== */

/**
 * Create a sequence grammar (AND of multiple grammars).
 * 
 * @param ctx The grammatica context
 * @param grammars Array of grammars to sequence (will be copied)
 * @param count Number of grammars in the array
 * @return Grammar representing the sequence, or NULL on error
 * 
 * Example:
 *   Grammar* parts[] = {hello_grammar, space_grammar, world_grammar};
 *   Grammar* sequence = grammatica_sequence(ctx, parts, 3);
 */
Grammar* grammatica_sequence(GrammaticaContextHandle_t ctx, Grammar** grammars, size_t count) {
	VALIDATE_PARAM_RET_NULL(ctx, grammars, "Grammars array cannot be NULL");
	
	if (count == 0) {
		grammatica_report_error(ctx, "Sequence must contain at least one grammar");
		return NULL;
	}
	
	/* Copy all input grammars */
	Grammar** copies = (Grammar**)malloc(count * sizeof(Grammar*));
	if (!copies) {
		grammatica_report_error(ctx, "Failed to allocate array for sequence copies");
		return NULL;
	}
	
	size_t i;
	for (i = 0; i < count; i++) {
		if (!grammars[i]) {
			/* Clean up previously copied grammars */
			size_t j;
			for (j = 0; j < i; j++) {
				grammatica_grammar_destroy(ctx, copies[j]);
			}
			free(copies);
			grammatica_report_error(ctx, "NULL grammar in sequence array");
			return NULL;
		}
		
		copies[i] = grammatica_grammar_copy(ctx, grammars[i]);
		if (!copies[i]) {
			/* Clean up previously copied grammars */
			size_t j;
			for (j = 0; j < i; j++) {
				grammatica_grammar_destroy(ctx, copies[j]);
			}
			free(copies);
			return NULL;
		}
	}
	
	/* Create AND with no quantifier */
	Quantifier quant = {1, 1};
	And* and_expr = grammatica_and_create(ctx, copies, count, quant);
	free(copies); /* and_create copies the array, so we can free ours */
	
	if (!and_expr) {
		return NULL;
	}
	
	/* Wrap in Grammar */
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_and_destroy(ctx, and_expr);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for sequence");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_AND;
	result->data = and_expr;
	return result;
}

/**
 * Create a choice grammar (OR of multiple grammars).
 * 
 * @param ctx The grammatica context
 * @param grammars Array of grammars to choose from (will be copied)
 * @param count Number of grammars in the array
 * @return Grammar representing the choice, or NULL on error
 * 
 * Example:
 *   Grammar* choices[] = {digit_grammar, alpha_grammar};
 *   Grammar* choice = grammatica_choice(ctx, choices, 2);
 */
Grammar* grammatica_choice(GrammaticaContextHandle_t ctx, Grammar** grammars, size_t count) {
	VALIDATE_PARAM_RET_NULL(ctx, grammars, "Grammars array cannot be NULL");
	
	if (count == 0) {
		grammatica_report_error(ctx, "Choice must contain at least one grammar");
		return NULL;
	}
	
	/* Copy all input grammars */
	Grammar** copies = (Grammar**)malloc(count * sizeof(Grammar*));
	if (!copies) {
		grammatica_report_error(ctx, "Failed to allocate array for choice copies");
		return NULL;
	}
	
	size_t i;
	for (i = 0; i < count; i++) {
		if (!grammars[i]) {
			/* Clean up previously copied grammars */
			size_t j;
			for (j = 0; j < i; j++) {
				grammatica_grammar_destroy(ctx, copies[j]);
			}
			free(copies);
			grammatica_report_error(ctx, "NULL grammar in choice array");
			return NULL;
		}
		
		copies[i] = grammatica_grammar_copy(ctx, grammars[i]);
		if (!copies[i]) {
			/* Clean up previously copied grammars */
			size_t j;
			for (j = 0; j < i; j++) {
				grammatica_grammar_destroy(ctx, copies[j]);
			}
			free(copies);
			return NULL;
		}
	}
	
	/* Create OR with no quantifier */
	Quantifier quant = {1, 1};
	Or* or_expr = grammatica_or_create(ctx, copies, count, quant);
	free(copies); /* or_create copies the array, so we can free ours */
	
	if (!or_expr) {
		return NULL;
	}
	
	/* Wrap in Grammar */
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		grammatica_or_destroy(ctx, or_expr);
		grammatica_report_error(ctx, "Failed to allocate Grammar wrapper for choice");
		return NULL;
	}
	
	result->type = GRAMMAR_TYPE_OR;
	result->data = or_expr;
	return result;
}
