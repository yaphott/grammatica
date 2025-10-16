#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_group.h"
#include "grammatica_internal.h"

static const char* OR_SEPARATOR = " | ";

/* Forward declarations */
extern Grammar* and_simplify_subexprs(GrammaticaContextHandle_t ctx, Grammar** subexprs, size_t num_subexprs, Quantifier quantifier);
static Grammar* or_simplify_subexprs(GrammaticaContextHandle_t ctx, Grammar** subexprs, size_t num_subexprs, Quantifier quantifier);
static bool or_needs_wrapped(const Or* or_expr);

/* Create Or */
Or* grammatica_or_create(GrammaticaContextHandle_t ctx, Grammar** subexprs, size_t num_subexprs, Quantifier quantifier) {
	if (!ctx || !subexprs) {
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
	Or* or_expr = (Or*)calloc(1, sizeof(Or));
	if (!or_expr) {
		grammatica_report_error(ctx, "Memory allocation failed");
		return NULL;
	}
	or_expr->subexprs = (Grammar**)malloc(num_subexprs * sizeof(Grammar*));
	if (!or_expr->subexprs && num_subexprs > 0) {
		free(or_expr);
		grammatica_report_error(ctx, "Memory allocation failed");
		return NULL;
	}
	memcpy(or_expr->subexprs, subexprs, num_subexprs * sizeof(Grammar*));
	or_expr->num_subexprs = num_subexprs;
	or_expr->quantifier = quantifier;
	return or_expr;
}

/* Destroy Or */
void grammatica_or_destroy(GrammaticaContextHandle_t ctx, Or* or_expr) {
	if (!or_expr) {
		return;
	}
	for (size_t i = 0; i < or_expr->num_subexprs; i++) {
		grammatica_grammar_destroy(ctx, or_expr->subexprs[i]);
	}
	free(or_expr->subexprs);
	free(or_expr);
}

/* Check if Or needs wrapping */
static bool or_needs_wrapped(const Or* or_expr) {
	size_t n = or_expr->num_subexprs;
	if (n < 1) {
		return false;
	}
	if (n == 1) {
		if (or_expr->quantifier.lower == 1 && or_expr->quantifier.upper == 1) {
			return false;
		}
		/* Recursively look for the first non-default (1, 1) subexpression */
		Grammar* subexpr = or_expr->subexprs[0];
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
	return true;
}

/* Render Or */
char* grammatica_or_render(GrammaticaContextHandle_t ctx, const Or* or_expr, bool full, bool wrap) {
	(void)full;
	if (!ctx || !or_expr) {
		return NULL;
	}
	if (or_expr->num_subexprs < 1) {
		return NULL;
	}
	char* rendered_quantifier = render_quantifier(or_expr->quantifier);
	char* result = (char*)malloc(8192);
	if (!result) {
		if (rendered_quantifier)
			free(rendered_quantifier);
		return NULL;
	}
	size_t pos = 0;
	bool found = false;
	for (size_t i = 0; i < or_expr->num_subexprs; i++) {
		char* rendered = grammatica_grammar_render(ctx, or_expr->subexprs[i], false, true);
		if (rendered) {
			if (found) {
				pos += snprintf(result + pos, 8192 - pos, "%s", OR_SEPARATOR);
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
	if (or_needs_wrapped(or_expr) && (wrap || rendered_quantifier)) {
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

/* Simplify Or - simplified version */
static Grammar* or_simplify_subexprs(GrammaticaContextHandle_t ctx, Grammar** original_subexprs, size_t original_num,
                                     Quantifier quantifier) {
	/* Simplify each subexpression and remove duplicates */
	Grammar** subexprs = (Grammar**)malloc(original_num * sizeof(Grammar*));
	if (!subexprs) {
		return NULL;
	}
	size_t n = 0;
	for (size_t i = 0; i < original_num; i++) {
		Grammar* simplified = grammatica_grammar_simplify(ctx, original_subexprs[i]);
		if (simplified) {
			/* Check for duplicates */
			bool duplicate = false;
			for (size_t j = 0; j < n; j++) {
				if (grammatica_grammar_equals(ctx, simplified, subexprs[j])) {
					grammatica_grammar_destroy(ctx, simplified);
					duplicate = true;
					break;
				}
			}
			if (!duplicate) {
				subexprs[n++] = simplified;
			}
		}
	}
	/* Empty expression is no-op */
	if (n < 1) {
		free(subexprs);
		return NULL;
	}
	/* Single subexpression */
	if (n == 1) {
		/* Unwrap a single default (1, 1) subexpression */
		if (quantifier.lower == 1 && quantifier.upper == 1) {
			Grammar* result = subexprs[0];
			free(subexprs);
			return result;
		}
		/* Or that is optional (0, 1) can recursively unwrap */
		if (quantifier.lower == 0 && quantifier.upper == 1) {
			if (subexprs[0]->type == GRAMMAR_TYPE_AND || subexprs[0]->type == GRAMMAR_TYPE_OR) {
				Quantifier sub_quant = {0, 0};
				if (subexprs[0]->type == GRAMMAR_TYPE_AND) {
					sub_quant = ((And*)subexprs[0]->data)->quantifier;
				} else {
					sub_quant = ((Or*)subexprs[0]->data)->quantifier;
				}
				if ((sub_quant.lower == 0 && sub_quant.upper == 1) || (sub_quant.lower == 1 && sub_quant.upper == 1)) {
					Grammar** sub_subexprs;
					size_t sub_num;
					if (subexprs[0]->type == GRAMMAR_TYPE_AND) {
						And* sub_and = (And*)subexprs[0]->data;
						sub_num = sub_and->num_subexprs;
						sub_subexprs = (Grammar**)malloc(sub_num * sizeof(Grammar*));
						if (sub_subexprs) {
							for (size_t i = 0; i < sub_num; i++) {
								sub_subexprs[i] = grammatica_grammar_copy(ctx, sub_and->subexprs[i]);
							}
						}
					} else {
						Or* sub_or = (Or*)subexprs[0]->data;
						sub_num = sub_or->num_subexprs;
						sub_subexprs = (Grammar**)malloc(sub_num * sizeof(Grammar*));
						if (sub_subexprs) {
							for (size_t i = 0; i < sub_num; i++) {
								sub_subexprs[i] = grammatica_grammar_copy(ctx, sub_or->subexprs[i]);
							}
						}
					}
					if (sub_subexprs) {
						grammatica_grammar_destroy(ctx, subexprs[0]);
						free(subexprs);
						/* Use And's simplify for single Or */
						Grammar* result = and_simplify_subexprs(ctx, sub_subexprs, sub_num, quantifier);
						for (size_t i = 0; i < sub_num; i++) {
							if (sub_subexprs[i])
								grammatica_grammar_destroy(ctx, sub_subexprs[i]);
						}
						free(sub_subexprs);
						return result;
					}
				}
			}
		}
		/* Or with a single subexpression is the same as And with a single subexpression */
		/* Use And's simplify_subexprs */
		Grammar* result = and_simplify_subexprs(ctx, subexprs, n, quantifier);
		for (size_t i = 0; i < n; i++) {
			if (subexprs[i])
				grammatica_grammar_destroy(ctx, subexprs[i]);
		}
		free(subexprs);
		return result;
	}
	/* Create Or with simplified subexpressions */
	Or* or_expr = grammatica_or_create(ctx, subexprs, n, quantifier);
	free(subexprs);
	if (!or_expr) {
		return NULL;
	}
	Grammar* grammar = (Grammar*)malloc(sizeof(Grammar));
	if (!grammar) {
		grammatica_or_destroy(ctx, or_expr);
		return NULL;
	}
	grammar->type = GRAMMAR_TYPE_OR;
	grammar->data = or_expr;
	return grammar;
}

Grammar* grammatica_or_simplify(GrammaticaContextHandle_t ctx, const Or* or_expr) {
	if (!ctx || !or_expr) {
		return NULL;
	}
	return or_simplify_subexprs(ctx, or_expr->subexprs, or_expr->num_subexprs, or_expr->quantifier);
}

/* Convert Or to string representation */
char* grammatica_or_as_string(GrammaticaContextHandle_t ctx, const Or* or_expr) {
	if (!ctx || !or_expr) {
		return NULL;
	}
	char* result = (char*)malloc(16384);
	if (!result) {
		return NULL;
	}
	size_t pos = 0;
	pos += snprintf(result + pos, 16384 - pos, "Or(subexprs=[");
	for (size_t i = 0; i < or_expr->num_subexprs; i++) {
		if (i > 0) {
			pos += snprintf(result + pos, 16384 - pos, ", ");
		}
		char* subexpr_str = grammatica_grammar_as_string(ctx, or_expr->subexprs[i]);
		if (subexpr_str) {
			pos += snprintf(result + pos, 16384 - pos, "%s", subexpr_str);
			grammatica_free_string(ctx, subexpr_str);
		}
	}
	if (or_expr->quantifier.upper == -1) {
		pos += snprintf(result + pos, 16384 - pos, "], quantifier=(%d, None))", or_expr->quantifier.lower);
	} else {
		pos += snprintf(result + pos, 16384 - pos, "], quantifier=(%d, %d))", or_expr->quantifier.lower, or_expr->quantifier.upper);
	}
	char* final_result = strdup(result);
	free(result);
	return final_result;
}

/* Check if two Ors are equal */
bool grammatica_or_equals(GrammaticaContextHandle_t ctx, const Or* a, const Or* b, bool check_quantifier) {
	if (!ctx) {
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

/* Copy Or */
Or* grammatica_or_copy(GrammaticaContextHandle_t ctx, const Or* or_expr) {
	if (!ctx || !or_expr) {
		return NULL;
	}
	Grammar** subexprs_copy = (Grammar**)malloc(or_expr->num_subexprs * sizeof(Grammar*));
	if (!subexprs_copy && or_expr->num_subexprs > 0) {
		return NULL;
	}
	for (size_t i = 0; i < or_expr->num_subexprs; i++) {
		subexprs_copy[i] = grammatica_grammar_copy(ctx, or_expr->subexprs[i]);
		if (!subexprs_copy[i]) {
			for (size_t j = 0; j < i; j++) {
				grammatica_grammar_destroy(ctx, subexprs_copy[j]);
			}
			free(subexprs_copy);
			return NULL;
		}
	}
	Or* result = grammatica_or_create(ctx, subexprs_copy, or_expr->num_subexprs, or_expr->quantifier);
	free(subexprs_copy);
	return result;
}

/* Get number of subexpressions */
size_t grammatica_or_get_num_subexprs(GrammaticaContextHandle_t ctx, const Or* or_expr) {
	if (!ctx || !or_expr) {
		return 0;
	}
	return or_expr->num_subexprs;
}

/* Get subexpressions */
int grammatica_or_get_subexprs(GrammaticaContextHandle_t ctx, const Or* or_expr, Grammar** out_subexprs, size_t max_subexprs) {
	if (!ctx || !or_expr || !out_subexprs) {
		return -1;
	}
	size_t copy_count = or_expr->num_subexprs < max_subexprs ? or_expr->num_subexprs : max_subexprs;
	memcpy(out_subexprs, or_expr->subexprs, copy_count * sizeof(Grammar*));
	return (int)copy_count;
}

/* Get quantifier */
Quantifier grammatica_or_get_quantifier(GrammaticaContextHandle_t ctx, const Or* or_expr) {
	Quantifier result = {0, 0};
	if (!ctx || !or_expr) {
		return result;
	}
	return or_expr->quantifier;
}
