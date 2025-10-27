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
	/* Initialize memory tracking */
	ctx->track_allocations = true;  /* Enabled by default */
	ctx->allocations_capacity = INITIAL_ALLOC_CAPACITY;
	ctx->allocations = (AllocationEntry*)malloc(ctx->allocations_capacity * sizeof(AllocationEntry));
	if (!ctx->allocations) {
		pthread_mutex_destroy(&ctx->mutex);
		free(ctx);
		return NULL;
	}
	ctx->num_allocations = 0;
	return ctx;
}

void grammatica_finish(GrammaticaContextHandle_t ctx) {
	if (!ctx) {
		return;
	}
	if (!grammatica_context_is_valid(ctx)) {
		return;
	}
	/* Free all tracked allocations */
	grammatica_free_all_tracked(ctx);
	pthread_mutex_lock(&ctx->mutex);
	ctx->magic = 0; /* Invalidate the context */
	ctx->initialized = 0;
	pthread_mutex_unlock(&ctx->mutex);
	pthread_mutex_destroy(&ctx->mutex);
	free(ctx);
}

void grammatica_set_error_handler(GrammaticaContextHandle_t ctx, GrammaticaErrorHandler handler, void* userdata) {
	if (!ctx) {
		return;
	}
	pthread_mutex_lock(&ctx->mutex);
	ctx->error_handler = handler;
	ctx->error_userdata = userdata;
	pthread_mutex_unlock(&ctx->mutex);
}

void grammatica_set_notice_handler(GrammaticaContextHandle_t ctx, GrammaticaNoticeHandler handler, void* userdata) {
	if (!ctx) {
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

void grammatica_report_error_with_code(GrammaticaContextHandle_t ctx, GrammaticaErrorCode code, const char* message) {
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
	if (!grammatica_context_is_valid(ctx)) {
		return "Invalid context";
	}
	pthread_mutex_lock(&ctx->mutex);
	const char* result = ctx->error_buffer[0] != '\0' ? ctx->error_buffer : NULL;
	pthread_mutex_unlock(&ctx->mutex);
	return result;
}

GrammaticaErrorCode grammatica_get_last_error_code(GrammaticaContextHandle_t ctx) {
	if (!grammatica_context_is_valid(ctx)) {
		return GRAMMATICA_ERROR_INVALID_CONTEXT;
	}
	pthread_mutex_lock(&ctx->mutex);
	GrammaticaErrorCode code = ctx->error_code;
	pthread_mutex_unlock(&ctx->mutex);
	return code;
}

const char* grammatica_error_code_to_string(GrammaticaErrorCode code) {
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
	if (!grammatica_context_is_valid(ctx) || !grammar) {
		return (GrammarType)-1;
	}
	return grammar->type;
}

void grammatica_grammar_destroy(GrammaticaContextHandle_t ctx, Grammar* grammar) {
	if (!grammatica_context_is_valid(ctx) || !grammar) {
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
				grammatica_char_range_destroy(ctx, (CharRange*)copied_data);
				break;
			case GRAMMAR_TYPE_STRING:
				grammatica_string_destroy(ctx, (String*)copied_data);
				break;
			case GRAMMAR_TYPE_DERIVATION_RULE:
				grammatica_derivation_rule_destroy(ctx, (DerivationRule*)copied_data);
				break;
			case GRAMMAR_TYPE_AND:
				grammatica_and_destroy(ctx, (And*)copied_data);
				break;
			case GRAMMAR_TYPE_OR:
				grammatica_or_destroy(ctx, (Or*)copied_data);
				break;
		}
		return NULL;
	}
	result->type = grammar->type;
	result->data = copied_data;
	return result;
}

void grammatica_free_string(GrammaticaContextHandle_t ctx, char* str) {
	(void)ctx; /* Unused in simple implementation */
	free(str);
}

/*
 * Memory Tracking Implementation
 */

static bool track_allocation(GrammaticaContext* ctx, void* ptr, size_t size, const char* file, int line) {
	if (!ctx->track_allocations || !ptr) {
		return true;
	}
	pthread_mutex_lock(&ctx->mutex);
	if (ctx->num_allocations >= ctx->allocations_capacity) {
		size_t new_capacity = ctx->allocations_capacity * 2;
		AllocationEntry* new_allocs = (AllocationEntry*)realloc(
			ctx->allocations, 
			new_capacity * sizeof(AllocationEntry)
		);
		if (!new_allocs) {
			pthread_mutex_unlock(&ctx->mutex);
			return false;
		}
		ctx->allocations = new_allocs;
		ctx->allocations_capacity = new_capacity;
	}
	ctx->allocations[ctx->num_allocations].ptr = ptr;
	ctx->allocations[ctx->num_allocations].size = size;
	ctx->allocations[ctx->num_allocations].file = file;
	ctx->allocations[ctx->num_allocations].line = line;
	ctx->num_allocations++;
	pthread_mutex_unlock(&ctx->mutex);
	return true;
}

static bool untrack_allocation(GrammaticaContext* ctx, void* ptr) {
	if (!ctx->track_allocations || !ptr) {
		return true;
	}
	pthread_mutex_lock(&ctx->mutex);
	for (size_t i = 0; i < ctx->num_allocations; i++) {
		if (ctx->allocations[i].ptr == ptr) {
			for (size_t j = i; j < ctx->num_allocations - 1; j++) {
				ctx->allocations[j] = ctx->allocations[j + 1];
			}
			ctx->num_allocations--;
			pthread_mutex_unlock(&ctx->mutex);
			return true;
		}
	}
	pthread_mutex_unlock(&ctx->mutex);
	return false;  /* Not found */
}

void* grammatica_tracked_malloc(GrammaticaContextHandle_t ctx, size_t size, const char* file, int line) {
	if (!grammatica_context_is_valid(ctx)) {
		return NULL;
	}
	void* ptr = malloc(size);
	if (!ptr) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Memory allocation failed");
		return NULL;
	}
	if (!track_allocation(ctx, ptr, size, file, line)) {
		free(ptr);
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Failed to track memory allocation");
		return NULL;
	}
	return ptr;
}

void* grammatica_tracked_calloc(GrammaticaContextHandle_t ctx, size_t nmemb, size_t size, const char* file, int line) {
	if (!grammatica_context_is_valid(ctx)) {
		return NULL;
	}
	void* ptr = calloc(nmemb, size);
	if (!ptr) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Memory allocation failed");
		return NULL;
	}
	if (!track_allocation(ctx, ptr, nmemb * size, file, line)) {
		free(ptr);
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Failed to track memory allocation");
		return NULL;
	}
	return ptr;
}

void* grammatica_tracked_realloc(GrammaticaContextHandle_t ctx, void* ptr, size_t size, const char* file, int line) {
	if (!grammatica_context_is_valid(ctx)) {
		return NULL;
	}
	if (!ptr) {
		return grammatica_tracked_malloc(ctx, size, file, line);
	}
	untrack_allocation(ctx, ptr);
	void* new_ptr = realloc(ptr, size);
	if (!new_ptr) {
		track_allocation(ctx, ptr, 0, file, line);  /* Size unknown */
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Memory reallocation failed");
		return NULL;
	}
	if (!track_allocation(ctx, new_ptr, size, file, line)) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Failed to track memory reallocation");
	}
	return new_ptr;
}

char* grammatica_tracked_strdup(GrammaticaContextHandle_t ctx, const char* s, const char* file, int line) {
	if (!grammatica_context_is_valid(ctx) || !s) {
		if (grammatica_context_is_valid(ctx) && !s) {
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_INVALID_PARAMETER,
				"Cannot duplicate NULL string");
		}
		return NULL;
	}
	size_t len = strlen(s) + 1;
	char* ptr = (char*)malloc(len);
	if (!ptr) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Memory allocation failed for string duplication");
		return NULL;
	}
	memcpy(ptr, s, len);
	if (!track_allocation(ctx, ptr, len, file, line)) {
		free(ptr);
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Failed to track string allocation");
		return NULL;
	}
	return ptr;
}

void grammatica_tracked_free(GrammaticaContextHandle_t ctx, void* ptr) {
	if (!ptr) {
		return;
	}
	if (grammatica_context_is_valid(ctx)) {
		untrack_allocation(ctx, ptr);
	}
	free(ptr);
}

void grammatica_free_all_tracked(GrammaticaContextHandle_t ctx) {
	if (ctx == NULL || ctx->allocations == NULL) {
		return;
	}
	pthread_mutex_lock(&ctx->mutex);
	for (size_t i = 0; i < ctx->num_allocations; i++) {
		if (ctx->allocations[i].ptr) {
			free(ctx->allocations[i].ptr);
		}
	}
	free(ctx->allocations);
	ctx->allocations = NULL;
	ctx->num_allocations = 0;
	ctx->allocations_capacity = 0;
	pthread_mutex_unlock(&ctx->mutex);
}
