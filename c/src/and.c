#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_group.h"
#include "grammatica_internal.h"

static const char* AND_SEPARATOR = " ";

Grammar* and_simplify_subexprs(GrammaticaContextHandle_t ctx, Grammar** subexprs, size_t num_subexprs, Quantifier quantifier);
static bool and_needs_wrapped(const And* and_expr);

And* grammatica_and_create(GrammaticaContextHandle_t ctx, Grammar** subexprs, size_t num_subexprs, Quantifier quantifier) {
	if (!grammatica_context_is_valid(ctx) || !subexprs) {
		return NULL;
	}
	if (quantifier.lower < 0) {
		grammatica_report_error(ctx, "Range lower bound must be non-negative");
		return NULL;
	}
	if (quantifier.upper != -1) {
		if (quantifier.upper < 1) {
			grammatica_report_error(ctx, "Range upper bound must be positive or -1 (infinity)");
			return NULL;
		}
		if (quantifier.lower > quantifier.upper) {
			grammatica_report_error(ctx, "Range lower bound must be <= range upper bound");
			return NULL;
		}
	}
	And* and_expr = (And*)calloc(1, sizeof(And));
	if (!and_expr) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	and_expr->subexprs = (Grammar**)malloc(num_subexprs * sizeof(Grammar*));
	if (!and_expr->subexprs && num_subexprs > 0) {
		free(and_expr);
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	memcpy(and_expr->subexprs, subexprs, num_subexprs * sizeof(Grammar*));
	and_expr->num_subexprs = num_subexprs;
	and_expr->quantifier = quantifier;
	return and_expr;
}

void grammatica_and_destroy(GrammaticaContextHandle_t ctx, And* and_expr) {
	if (!and_expr) {
		return;
	}
	for (size_t i = 0; i < and_expr->num_subexprs; i++) {
		grammatica_grammar_destroy(ctx, and_expr->subexprs[i]);
	}
	free(and_expr->subexprs);
	free(and_expr);
}

static bool and_needs_wrapped(const And* and_expr) {
	size_t n = and_expr->num_subexprs;
	if (n < 1) {
		return false;
	}
	if (n == 1) {
		if (and_expr->quantifier.lower == 1 && and_expr->quantifier.upper == 1) {
			return false;
		}
		/* Recursively look for the first non-default (1, 1) subexpression */
		Grammar* subexpr = and_expr->subexprs[0];
		bool wrap = (subexpr->type == GRAMMAR_TYPE_AND || subexpr->type == GRAMMAR_TYPE_OR);
		while (wrap) {
			if (subexpr->type == GRAMMAR_TYPE_AND) {
				And* sub_and = (And*)subexpr->data;
				if (!(sub_and->quantifier.lower == 1 && sub_and->quantifier.upper == 1) || sub_and->num_subexprs != 1) {
					break;
				}
				subexpr = sub_and->subexprs[0];
			} else if (subexpr->type == GRAMMAR_TYPE_OR) {
				Or* sub_or = (Or*)subexpr->data;
				if (!(sub_or->quantifier.lower == 1 && sub_or->quantifier.upper == 1) || sub_or->num_subexprs != 1) {
					break;
				}
				subexpr = sub_or->subexprs[0];
			} else {
				wrap = false;
			}
		}
		return wrap;
	}
	return !(and_expr->quantifier.lower == 1 && and_expr->quantifier.upper == 1);
}

char* grammatica_and_render(GrammaticaContextHandle_t ctx, const And* and_expr, bool full, bool wrap) {
	(void)full;
	if (!grammatica_context_is_valid(ctx) || !and_expr) {
		return NULL;
	}
	if (and_expr->num_subexprs < 1) {
		return NULL;
	}
	char* rendered_quantifier = render_quantifier(and_expr->quantifier);
	char* result = (char*)malloc(8192);
	if (!result) {
		if (rendered_quantifier)
			free(rendered_quantifier);
		return NULL;
	}
	size_t pos = 0;
	bool found = false;
	for (size_t i = 0; i < and_expr->num_subexprs; i++) {
		char* rendered = grammatica_grammar_render(ctx, and_expr->subexprs[i], false, true);
		if (rendered) {
			if (found) {
				pos += snprintf(result + pos, 8192 - pos, "%s", AND_SEPARATOR);
			}
			pos += snprintf(result + pos, 8192 - pos, "%s", rendered);
			grammatica_free_string(ctx, rendered);
			found = true;
		}
	}
	if (!found) {
		if (rendered_quantifier)
			free(rendered_quantifier);
		free(result);
		return NULL;
	}
	if (and_needs_wrapped(and_expr) && (wrap || rendered_quantifier)) {
		char* temp = (char*)malloc(8192);
		if (temp) {
			snprintf(temp, 8192, "(%s)", result);
			free(result);
			result = temp;
		}
	}
	if (rendered_quantifier) {
		char* temp = (char*)malloc(8192);
		if (temp) {
			snprintf(temp, 8192, "%s%s", result, rendered_quantifier);
			free(result);
			result = temp;
		}
		free(rendered_quantifier);
	}
	char* final_result = strdup(result);
	free(result);
	return final_result;
}

Grammar* and_simplify_subexprs(GrammaticaContextHandle_t ctx, Grammar** original_subexprs, size_t original_num, Quantifier quantifier) {
	Grammar** subexprs = NULL;
	Grammar** sub_subexprs = NULL;
	And* and_expr = NULL;
	Grammar* grammar = NULL;
	Grammar* result = NULL;
	
	/* Simplify each subexpression */
	subexprs = (Grammar**)malloc(original_num * sizeof(Grammar*));
	if (!subexprs) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		goto cleanup;
	}
	
	size_t n = 0;
	for (size_t i = 0; i < original_num; i++) {
		Grammar* simplified = grammatica_grammar_simplify(ctx, original_subexprs[i]);
		if (simplified) {
			subexprs[n++] = simplified;
		}
	}
	
	/* Empty expression is no-op */
	if (n < 1) {
		goto cleanup;
	}
	
	/* Single subexpression with default quantifier */
	if (n == 1 && quantifier.lower == 1 && quantifier.upper == 1) {
		result = subexprs[0];
		subexprs[0] = NULL; /* Prevent cleanup from freeing it */
		goto cleanup;
	}
	
	/* And that is optional (0, 1) can recursively unwrap */
	if (n == 1 && quantifier.lower == 0 && quantifier.upper == 1) {
		if (subexprs[0]->type == GRAMMAR_TYPE_AND) {
			And* sub_and = (And*)subexprs[0]->data;
			if ((sub_and->quantifier.lower == 0 && sub_and->quantifier.upper == 1) ||
			    (sub_and->quantifier.lower == 1 && sub_and->quantifier.upper == 1)) {
				sub_subexprs = (Grammar**)malloc(sub_and->num_subexprs * sizeof(Grammar*));
				if (sub_subexprs) {
					size_t sub_num = sub_and->num_subexprs;
					for (size_t i = 0; i < sub_num; i++) {
						sub_subexprs[i] = grammatica_grammar_copy(ctx, sub_and->subexprs[i]);
						if (!sub_subexprs[i]) {
							/* Cleanup already copied subexprs */
							for (size_t j = 0; j < i; j++) {
								if (sub_subexprs[j]) {
									grammatica_grammar_destroy(ctx, sub_subexprs[j]);
								}
							}
							free(sub_subexprs);
							sub_subexprs = NULL;
							goto cleanup;
						}
					}
					grammatica_grammar_destroy(ctx, subexprs[0]);
					subexprs[0] = NULL;
					result = and_simplify_subexprs(ctx, sub_subexprs, sub_num, quantifier);
					/* Cleanup sub_subexprs array */
					for (size_t i = 0; i < sub_num; i++) {
						if (sub_subexprs[i]) {
							grammatica_grammar_destroy(ctx, sub_subexprs[i]);
						}
					}
					free(sub_subexprs);
					sub_subexprs = NULL;
					goto cleanup;
				}
			}
		}
	}
	
	/* Create And with simplified subexpressions */
	and_expr = grammatica_and_create(ctx, subexprs, n, quantifier);
	if (!and_expr) {
		grammatica_report_error(ctx, "Failed to create And expression");
		goto cleanup;
	}
	
	grammar = (Grammar*)malloc(sizeof(Grammar));
	if (!grammar) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		goto cleanup;
	}
	
	grammar->type = GRAMMAR_TYPE_AND;
	grammar->data = and_expr;
	and_expr = NULL; /* Prevent cleanup from freeing it */
	result = grammar;
	grammar = NULL;
	
cleanup:
	if (subexprs) {
		for (size_t i = 0; i < n; i++) {
			if (subexprs[i]) {
				grammatica_grammar_destroy(ctx, subexprs[i]);
			}
		}
		free(subexprs);
	}
	if (and_expr) {
		grammatica_and_destroy(ctx, and_expr);
	}
	if (grammar) {
		free(grammar);
	}
	return result;
}

Grammar* grammatica_and_simplify(GrammaticaContextHandle_t ctx, const And* and_expr) {
	if (!grammatica_context_is_valid(ctx) || !and_expr) {
		return NULL;
	}
	return and_simplify_subexprs(ctx, and_expr->subexprs, and_expr->num_subexprs, and_expr->quantifier);
}

char* grammatica_and_as_string(GrammaticaContextHandle_t ctx, const And* and_expr) {
	if (!grammatica_context_is_valid(ctx) || !and_expr) {
		return NULL;
	}
	char* result = (char*)malloc(16384);
	if (!result) {
		return NULL;
	}
	size_t pos = 0;
	pos += snprintf(result + pos, 16384 - pos, "And(subexprs=[");
	for (size_t i = 0; i < and_expr->num_subexprs; i++) {
		if (i > 0) {
			pos += snprintf(result + pos, 16384 - pos, ", ");
		}
		char* subexpr_str = grammatica_grammar_as_string(ctx, and_expr->subexprs[i]);
		if (subexpr_str) {
			pos += snprintf(result + pos, 16384 - pos, "%s", subexpr_str);
			grammatica_free_string(ctx, subexpr_str);
		}
	}
	if (and_expr->quantifier.upper == -1) {
		pos += snprintf(result + pos, 16384 - pos, "], quantifier=(%d, None))", and_expr->quantifier.lower);
	} else {
		pos += snprintf(result + pos, 16384 - pos, "], quantifier=(%d, %d))", and_expr->quantifier.lower, and_expr->quantifier.upper);
	}
	char* final_result = strdup(result);
	free(result);
	return final_result;
}

bool grammatica_and_equals(GrammaticaContextHandle_t ctx, const And* a, const And* b, bool check_quantifier) {
	if (!grammatica_context_is_valid(ctx)) {
		return false;
	}
	if (a == b) {
		return true;
	}
	if (!a || !b) {
		return false;
	}
	if (check_quantifier) {
		if (a->quantifier.lower != b->quantifier.lower || a->quantifier.upper != b->quantifier.upper) {
			return false;
		}
	}
	if (a->num_subexprs != b->num_subexprs) {
		return false;
	}
	for (size_t i = 0; i < a->num_subexprs; i++) {
		if (!grammatica_grammar_equals(ctx, a->subexprs[i], b->subexprs[i])) {
			return false;
		}
	}
	return true;
}

And* grammatica_and_copy(GrammaticaContextHandle_t ctx, const And* and_expr) {
	if (!grammatica_context_is_valid(ctx) || !and_expr) {
		return NULL;
	}
	Grammar** subexprs_copy = (Grammar**)malloc(and_expr->num_subexprs * sizeof(Grammar*));
	if (!subexprs_copy && and_expr->num_subexprs > 0) {
		return NULL;
	}
	for (size_t i = 0; i < and_expr->num_subexprs; i++) {
		subexprs_copy[i] = grammatica_grammar_copy(ctx, and_expr->subexprs[i]);
		if (!subexprs_copy[i]) {
			for (size_t j = 0; j < i; j++) {
				grammatica_grammar_destroy(ctx, subexprs_copy[j]);
			}
			free(subexprs_copy);
			return NULL;
		}
	}
	And* result = grammatica_and_create(ctx, subexprs_copy, and_expr->num_subexprs, and_expr->quantifier);
	free(subexprs_copy);
	return result;
}

size_t grammatica_and_get_num_subexprs(GrammaticaContextHandle_t ctx, const And* and_expr) {
	if (!grammatica_context_is_valid(ctx) || !and_expr) {
		return 0;
	}
	return and_expr->num_subexprs;
}

int grammatica_and_get_subexprs(GrammaticaContextHandle_t ctx, const And* and_expr, Grammar** out_subexprs, size_t max_subexprs) {
	if (!grammatica_context_is_valid(ctx) || !and_expr || !out_subexprs) {
		return -1;
	}
	size_t copy_count = and_expr->num_subexprs < max_subexprs ? and_expr->num_subexprs : max_subexprs;
	memcpy(out_subexprs, and_expr->subexprs, copy_count * sizeof(Grammar*));
	return (int)copy_count;
}

Quantifier grammatica_and_get_quantifier(GrammaticaContextHandle_t ctx, const And* and_expr) {
	Quantifier result = {0, 0};
	if (!grammatica_context_is_valid(ctx) || !and_expr) {
		return result;
	}
	return and_expr->quantifier;
}
