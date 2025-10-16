#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_internal.h"

/* Context management */
GrammaticaContextHandle_t grammatica_init(void) {
	GrammaticaContext* ctx = (GrammaticaContext*)calloc(1, sizeof(GrammaticaContext));
	if (!ctx) {
		return NULL;
	}
	pthread_mutex_init(&ctx->mutex, NULL);
	ctx->initialized = 1;
	ctx->error_handler = NULL;
	ctx->error_userdata = NULL;
	ctx->notice_handler = NULL;
	ctx->notice_userdata = NULL;
	return ctx;
}

void grammatica_finish(GrammaticaContextHandle_t ctx) {
	if (!ctx) {
		return;
	}
	pthread_mutex_destroy(&ctx->mutex);
	free(ctx);
}

void grammatica_set_error_handler(GrammaticaContextHandle_t ctx, GrammaticaErrorHandler handler, void* userdata) {
	if (!ctx) {
		return;
	}
	ctx->error_handler = handler;
	ctx->error_userdata = userdata;
}

void grammatica_set_notice_handler(GrammaticaContextHandle_t ctx, GrammaticaNoticeHandler handler, void* userdata) {
	if (!ctx) {
		return;
	}
	ctx->notice_handler = handler;
	ctx->notice_userdata = userdata;
}

/* Helper functions for error reporting */
void grammatica_report_error(GrammaticaContextHandle_t ctx, const char* message) {
	if (!ctx) {
		return;
	}
	if (ctx->error_handler) {
		ctx->error_handler(message, ctx->error_userdata);
	}
}

void grammatica_report_notice(GrammaticaContextHandle_t ctx, const char* message) {
	if (!ctx) {
		return;
	}
	if (ctx->notice_handler) {
		ctx->notice_handler(message, ctx->notice_userdata);
	}
}

/* Generic grammar operations */
GrammarType grammatica_grammar_get_type(GrammaticaContextHandle_t ctx, const Grammar* grammar) {
	if (!ctx || !grammar) {
		return (GrammarType)-1;
	}
	return grammar->type;
}

void grammatica_grammar_destroy(GrammaticaContextHandle_t ctx, Grammar* grammar) {
	if (!ctx || !grammar) {
		return;
	}
	switch (grammar->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			grammatica_char_range_destroy(ctx, (CharRange*)grammar->data);
			break;
		case GRAMMAR_TYPE_STRING:
			grammatica_string_destroy(ctx, (String*)grammar->data);
			break;
		case GRAMMAR_TYPE_DERIVATION_RULE:
			grammatica_derivation_rule_destroy(ctx, (DerivationRule*)grammar->data);
			break;
		case GRAMMAR_TYPE_AND:
			grammatica_and_destroy(ctx, (And*)grammar->data);
			break;
		case GRAMMAR_TYPE_OR:
			grammatica_or_destroy(ctx, (Or*)grammar->data);
			break;
	}
	free(grammar);
}

char* grammatica_grammar_render(GrammaticaContextHandle_t ctx, const Grammar* grammar, bool full, bool wrap) {
	if (!ctx || !grammar) {
		return NULL;
	}
	switch (grammar->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			return grammatica_char_range_render(ctx, (CharRange*)grammar->data, full, wrap);
		case GRAMMAR_TYPE_STRING:
			return grammatica_string_render(ctx, (String*)grammar->data, full, wrap);
		case GRAMMAR_TYPE_DERIVATION_RULE:
			return grammatica_derivation_rule_render(ctx, (DerivationRule*)grammar->data, full, wrap);
		case GRAMMAR_TYPE_AND:
			return grammatica_and_render(ctx, (And*)grammar->data, full, wrap);
		case GRAMMAR_TYPE_OR:
			return grammatica_or_render(ctx, (Or*)grammar->data, full, wrap);
		default:
			return NULL;
	}
}

Grammar* grammatica_grammar_simplify(GrammaticaContextHandle_t ctx, const Grammar* grammar) {
	if (!ctx || !grammar) {
		return NULL;
	}
	switch (grammar->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			return grammatica_char_range_simplify(ctx, (CharRange*)grammar->data);
		case GRAMMAR_TYPE_STRING:
			return grammatica_string_simplify(ctx, (String*)grammar->data);
		case GRAMMAR_TYPE_DERIVATION_RULE:
			return grammatica_derivation_rule_simplify(ctx, (DerivationRule*)grammar->data);
		case GRAMMAR_TYPE_AND:
			return grammatica_and_simplify(ctx, (And*)grammar->data);
		case GRAMMAR_TYPE_OR:
			return grammatica_or_simplify(ctx, (Or*)grammar->data);
		default:
			return NULL;
	}
}

char* grammatica_grammar_as_string(GrammaticaContextHandle_t ctx, const Grammar* grammar) {
	if (!ctx || !grammar) {
		return NULL;
	}
	switch (grammar->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			return grammatica_char_range_as_string(ctx, (CharRange*)grammar->data);
		case GRAMMAR_TYPE_STRING:
			return grammatica_string_as_string(ctx, (String*)grammar->data);
		case GRAMMAR_TYPE_DERIVATION_RULE:
			return grammatica_derivation_rule_as_string(ctx, (DerivationRule*)grammar->data);
		case GRAMMAR_TYPE_AND:
			return grammatica_and_as_string(ctx, (And*)grammar->data);
		case GRAMMAR_TYPE_OR:
			return grammatica_or_as_string(ctx, (Or*)grammar->data);
		default:
			return NULL;
	}
}

bool grammatica_grammar_equals(GrammaticaContextHandle_t ctx, const Grammar* a, const Grammar* b) {
	if (!ctx) {
		return false;
	}
	if (a == b) {
		return true;
	}
	if (!a || !b) {
		return false;
	}
	if (a->type != b->type) {
		return false;
	}
	switch (a->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			return grammatica_char_range_equals(ctx, (CharRange*)a->data, (CharRange*)b->data);
		case GRAMMAR_TYPE_STRING:
			return grammatica_string_equals(ctx, (String*)a->data, (String*)b->data);
		case GRAMMAR_TYPE_DERIVATION_RULE:
			return grammatica_derivation_rule_equals(ctx, (DerivationRule*)a->data, (DerivationRule*)b->data);
		case GRAMMAR_TYPE_AND:
			return grammatica_and_equals(ctx, (And*)a->data, (And*)b->data, true);
		case GRAMMAR_TYPE_OR:
			return grammatica_or_equals(ctx, (Or*)a->data, (Or*)b->data, true);
		default:
			return false;
	}
}

Grammar* grammatica_grammar_copy(GrammaticaContextHandle_t ctx, const Grammar* grammar) {
	if (!ctx || !grammar) {
		return NULL;
	}
	switch (grammar->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			return (Grammar*)grammatica_char_range_copy(ctx, (CharRange*)grammar->data);
		case GRAMMAR_TYPE_STRING:
			return (Grammar*)grammatica_string_copy(ctx, (String*)grammar->data);
		case GRAMMAR_TYPE_DERIVATION_RULE:
			return (Grammar*)grammatica_derivation_rule_copy(ctx, (DerivationRule*)grammar->data);
		case GRAMMAR_TYPE_AND:
			return (Grammar*)grammatica_and_copy(ctx, (And*)grammar->data);
		case GRAMMAR_TYPE_OR:
			return (Grammar*)grammatica_or_copy(ctx, (Or*)grammar->data);
		default:
			return NULL;
	}
}

/* Utility functions */
void grammatica_free_string(GrammaticaContextHandle_t ctx, char* str) {
	(void)ctx; /* Unused in simple implementation */
	free(str);
}
