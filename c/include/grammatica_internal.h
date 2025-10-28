#ifndef GRAMMATICA_INTERNAL_H
#define GRAMMATICA_INTERNAL_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#include "grammatica.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GRAMMATICA_MAGIC 0x47524D4D /* "GRMM" */
#define INITIAL_ALLOC_CAPACITY 64

/* Memory allocation tracking entry */
typedef struct AllocationEntry_t {
	void* ptr;
	size_t size;
	const char* file;
	int line;
} AllocationEntry;

typedef struct GrammaticaContext_t {
	uint32_t magic;
	GrammaticaErrorHandler error_handler;
	void* error_userdata;
	GrammaticaNoticeHandler notice_handler;
	void* notice_userdata;
	char error_buffer[1024];
	GrammaticaError_t error_code;
	pthread_mutex_t mutex;
	int initialized;

	// /* Memory tracking */
	// AllocationEntry* allocations;
	// size_t num_allocations;
	// size_t allocations_capacity;
	// bool track_allocations; /* Enable/disable tracking */
} GrammaticaContext;

struct Grammar_t {
	GrammarType type;
	void* data;
};

struct CharRange_t {
	CharRangePair* ranges;
	size_t ranges_n;
	bool negate;
};

struct String_t {
	char* value;
};

struct DerivationRule_t {
	char* symbol;
	Grammar* value;
};

struct And_t {
	Grammar** subexprs;
	size_t subexprs_n;
	Quantifier quantifier;
};

struct Or_t {
	Grammar** subexprs;
	size_t subexprs_n;
	Quantifier quantifier;
};

void grammatica_report_error(GrammaticaContextHandle_t ctx, const char* message);
void grammatica_report_error_with_code(GrammaticaContextHandle_t ctx, GrammaticaError_t code, const char* message);
void grammatica_report_notice(GrammaticaContextHandle_t ctx, const char* message);

/* Convenience macros for tracked allocations */
#define GRAMMATICA_MALLOC(ctx, size) grammatica_tracked_malloc(ctx, size, __FILE__, __LINE__)
#define GRAMMATICA_CALLOC(ctx, nmemb, size) grammatica_tracked_calloc(ctx, nmemb, size, __FILE__, __LINE__)
#define GRAMMATICA_REALLOC(ctx, ptr, size) grammatica_tracked_realloc(ctx, ptr, size, __FILE__, __LINE__)
#define GRAMMATICA_STRDUP(ctx, s) grammatica_tracked_strdup(ctx, s, __FILE__, __LINE__)
#define GRAMMATICA_FREE(ctx, ptr) grammatica_tracked_free(ctx, ptr)

static inline bool grammatica_context_is_valid(const GrammaticaContext* ctx) {
	return ctx && ctx->magic == GRAMMATICA_MAGIC && ctx->initialized;
}

/* Validation helper macros for consistent error checking */
#define VALIDATE_CONTEXT_RET_NULL(ctx)           \
	do {                                         \
		if (!grammatica_context_is_valid(ctx)) { \
			return NULL;                         \
		}                                        \
	} while (0)

#define VALIDATE_CONTEXT_RET_FALSE(ctx)          \
	do {                                         \
		if (!grammatica_context_is_valid(ctx)) { \
			return false;                        \
		}                                        \
	} while (0)

#define VALIDATE_CONTEXT_RET_VOID(ctx)           \
	do {                                         \
		if (!grammatica_context_is_valid(ctx)) { \
			return;                              \
		}                                        \
	} while (0)

#define VALIDATE_PARAM_RET_NULL(ctx, param, msg)                                             \
	do {                                                                                     \
		if (!(param)) {                                                                      \
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_INVALID_PARAMETER, msg); \
			return NULL;                                                                     \
		}                                                                                    \
	} while (0)

#define VALIDATE_PARAM_RET_FALSE(ctx, param, msg)                                            \
	do {                                                                                     \
		if (!(param)) {                                                                      \
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_INVALID_PARAMETER, msg); \
			return false;                                                                    \
		}                                                                                    \
	} while (0)

#define VALIDATE_CONTEXT_RET_ERRCODE(ctx)        \
	do {                                         \
		if (!grammatica_context_is_valid(ctx)) { \
			return GRAMMATICA_ERROR_INVALID_CONTEXT; \
		}                                        \
	} while (0)

#define VALIDATE_PARAM_RET_ERRCODE(ctx, param, msg)                                          \
	do {                                                                                     \
		if (!(param)) {                                                                      \
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_INVALID_PARAMETER, msg); \
			return GRAMMATICA_ERROR_INVALID_PARAMETER;                                       \
		}                                                                                    \
	} while (0)

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_INTERNAL_H */
