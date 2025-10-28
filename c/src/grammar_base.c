#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_internal.h"

GrammaticaContextHandle_t grammatica_init(void) {
	GrammaticaContext* ctx = (GrammaticaContext*)calloc(1, sizeof(GrammaticaContext));
	if (!ctx) {
		return NULL;
	}
	if (pthread_mutex_init(&ctx->mutex, NULL) != 0) {
		free(ctx);
		return NULL;
	}
	ctx->magic = GRAMMATICA_MAGIC;
	ctx->initialized = 1;
	ctx->error_handler = NULL;
	ctx->error_userdata = NULL;
	ctx->notice_handler = NULL;
	ctx->notice_userdata = NULL;
	ctx->error_code = GRAMMATICA_ERROR_NONE;
	// /* Initialize memory tracking */
	// ctx->track_allocations = true; /* Enabled by default */
	// ctx->allocations_capacity = INITIAL_ALLOC_CAPACITY;
	// ctx->allocations = (AllocationEntry*)malloc(ctx->allocations_capacity * sizeof(AllocationEntry));
	// if (!ctx->allocations) {
	// 	pthread_mutex_destroy(&ctx->mutex);
	// 	free(ctx);
	// 	return NULL;
	// }
	// ctx->num_allocations = 0;
	return ctx;
}

void grammatica_finish(GrammaticaContextHandle_t ctx) {
	if (ctx == NULL) {
		return;
	}
	// if (!grammatica_context_is_valid(ctx)) {
	// 	return;
	// }
	pthread_mutex_lock(&ctx->mutex);
	ctx->magic = 0; /* Invalidate the context */
	ctx->initialized = 0;
	pthread_mutex_unlock(&ctx->mutex);
	pthread_mutex_destroy(&ctx->mutex);
	free(ctx);
}

void grammatica_set_error_handler(GrammaticaContextHandle_t ctx, GrammaticaErrorHandler handler, void* userdata) {
	if (ctx == NULL) {
		return;
	}
	pthread_mutex_lock(&ctx->mutex);
	ctx->error_handler = handler;
	ctx->error_userdata = userdata;
	pthread_mutex_unlock(&ctx->mutex);
}

void grammatica_set_notice_handler(GrammaticaContextHandle_t ctx, GrammaticaNoticeHandler handler, void* userdata) {
	if (ctx == NULL) {
		return;
	}
	pthread_mutex_lock(&ctx->mutex);
	ctx->notice_handler = handler;
	ctx->notice_userdata = userdata;
	pthread_mutex_unlock(&ctx->mutex);
}

void grammatica_report_error(GrammaticaContextHandle_t ctx, const char* message) {
	/* Default to unknown error code for backward compatibility */
	grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_UNKNOWN, message);
}

void grammatica_report_error_with_code(GrammaticaContextHandle_t ctx, GrammaticaError_t code, const char* message) {
	if (ctx == NULL) {
		return;
	}
	pthread_mutex_lock(&ctx->mutex);
	ctx->error_code = code;
	if (ctx->error_handler) {
		ctx->error_handler(message, ctx->error_userdata);
	} else {
		/* Store in error_buffer for later retrieval */
		strncpy(ctx->error_buffer, message, sizeof(ctx->error_buffer) - 1);
		ctx->error_buffer[sizeof(ctx->error_buffer) - 1] = '\0';
	}
	pthread_mutex_unlock(&ctx->mutex);
}

void grammatica_report_notice(GrammaticaContextHandle_t ctx, const char* message) {
	if (ctx == NULL) {
		return;
	}
	pthread_mutex_lock(&ctx->mutex);
	if (ctx->notice_handler) {
		ctx->notice_handler(message, ctx->notice_userdata);
	}
	pthread_mutex_unlock(&ctx->mutex);
}

const char* grammatica_get_last_error(GrammaticaContextHandle_t ctx) {
	if (ctx == NULL) {
		return NULL;
	}
	if (!grammatica_context_is_valid(ctx)) {
		return "Invalid context";
	}
	pthread_mutex_lock(&ctx->mutex);
	const char* result = ctx->error_buffer[0] != '\0' ? ctx->error_buffer : NULL;
	pthread_mutex_unlock(&ctx->mutex);
	return result;
}

GrammaticaError_t grammatica_get_last_error_code(GrammaticaContextHandle_t ctx) {
	if (!grammatica_context_is_valid(ctx)) {
		return GRAMMATICA_ERROR_INVALID_CONTEXT;
	}
	pthread_mutex_lock(&ctx->mutex);
	GrammaticaError_t code = ctx->error_code;
	pthread_mutex_unlock(&ctx->mutex);
	return code;
}

const char* grammatica_error_code_to_string(GrammaticaError_t code) {
	switch (code) {
		case GRAMMATICA_ERROR_NONE:
			return "No error";
		case GRAMMATICA_ERROR_INVALID_CONTEXT:
			return "Invalid context";
		case GRAMMATICA_ERROR_INVALID_PARAMETER:
			return "Invalid parameter";
		case GRAMMATICA_ERROR_OUT_OF_MEMORY:
			return "Out of memory";
		case GRAMMATICA_ERROR_INVALID_GRAMMAR:
			return "Invalid grammar";
		case GRAMMATICA_ERROR_SIMPLIFICATION:
			return "Simplification error";
		case GRAMMATICA_ERROR_RENDER:
			return "Render error";
		case GRAMMATICA_ERROR_COPY:
			return "Copy error";
		case GRAMMATICA_ERROR_UNKNOWN:
		default:
			return "Unknown error";
	}
}

void grammatica_clear_error(GrammaticaContextHandle_t ctx) {
	if (!grammatica_context_is_valid(ctx)) {
		return;
	}
	pthread_mutex_lock(&ctx->mutex);
	ctx->error_code = GRAMMATICA_ERROR_NONE;
	ctx->error_buffer[0] = '\0';
	pthread_mutex_unlock(&ctx->mutex);
}

GrammarType grammatica_grammar_get_type(GrammaticaContextHandle_t ctx, const Grammar* grammar) {
	if (!grammatica_context_is_valid(ctx) || grammar == NULL) {
		return (GrammarType)-1;
	}
	return grammar->type;
}

void grammatica_grammar_destroy(Grammar* grammar) {
	// if (!grammatica_context_is_valid(ctx) || grammar == NULL) {
	// 	return;
	// }
	switch (grammar->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			grammatica_char_range_destroy((CharRange*)grammar->data);
			break;
		case GRAMMAR_TYPE_STRING:
			grammatica_string_destroy((String*)grammar->data);
			break;
		case GRAMMAR_TYPE_DERIVATION_RULE:
			grammatica_derivation_rule_destroy((DerivationRule*)grammar->data);
			break;
		case GRAMMAR_TYPE_AND:
			grammatica_and_destroy((And*)grammar->data);
			break;
		case GRAMMAR_TYPE_OR:
			grammatica_or_destroy((Or*)grammar->data);
			break;
	}
	free(grammar);
}

char* grammatica_grammar_render(GrammaticaContextHandle_t ctx, const Grammar* grammar, bool full, bool wrap) {
	if (!grammatica_context_is_valid(ctx) || !grammar) {
		return NULL;
	}
	switch (grammar->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			return grammatica_char_range_render(ctx, (CharRange*)grammar->data, full, wrap);
		case GRAMMAR_TYPE_STRING:
			return grammatica_string_render(ctx, (String*)grammar->data);
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
	if (!grammatica_context_is_valid(ctx) || !grammar) {
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
	if (!grammatica_context_is_valid(ctx) || !grammar) {
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
	if (!grammatica_context_is_valid(ctx)) {
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
	if (!grammatica_context_is_valid(ctx) || !grammar) {
		return NULL;
	}
	void* copied_data = NULL;
	switch (grammar->type) {
		case GRAMMAR_TYPE_CHAR_RANGE:
			copied_data = grammatica_char_range_copy(ctx, (CharRange*)grammar->data);
			break;
		case GRAMMAR_TYPE_STRING:
			copied_data = grammatica_string_copy(ctx, (String*)grammar->data);
			break;
		case GRAMMAR_TYPE_DERIVATION_RULE:
			copied_data = grammatica_derivation_rule_copy(ctx, (DerivationRule*)grammar->data);
			break;
		case GRAMMAR_TYPE_AND:
			copied_data = grammatica_and_copy(ctx, (And*)grammar->data);
			break;
		case GRAMMAR_TYPE_OR:
			copied_data = grammatica_or_copy(ctx, (Or*)grammar->data);
			break;
		default:
			return NULL;
	}
	if (!copied_data) {
		return NULL;
	}
	/* Wrap in Grammar structure */
	Grammar* result = (Grammar*)malloc(sizeof(Grammar));
	if (!result) {
		/* Clean up copied data based on type */
		switch (grammar->type) {
			case GRAMMAR_TYPE_CHAR_RANGE:
				grammatica_char_range_destroy((CharRange*)copied_data);
				break;
			case GRAMMAR_TYPE_STRING:
				grammatica_string_destroy((String*)copied_data);
				break;
			case GRAMMAR_TYPE_DERIVATION_RULE:
				grammatica_derivation_rule_destroy((DerivationRule*)copied_data);
				break;
			case GRAMMAR_TYPE_AND:
				grammatica_and_destroy((And*)copied_data);
				break;
			case GRAMMAR_TYPE_OR:
				grammatica_or_destroy((Or*)copied_data);
				break;
		}
		return NULL;
	}
	result->type = grammar->type;
	result->data = copied_data;
	return result;
}