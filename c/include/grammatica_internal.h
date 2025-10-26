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

typedef struct GrammaticaContext_t {
	uint32_t magic;
	GrammaticaErrorHandler error_handler;
	void* error_userdata;
	GrammaticaNoticeHandler notice_handler;
	void* notice_userdata;
	char error_buffer[1024];
	GrammaticaErrorCode error_code;
	pthread_mutex_t mutex;
	int initialized;
} GrammaticaContext;

struct Grammar_t {
	GrammarType type;
	void* data;
};

struct CharRange_t {
	CharRangePair* ranges;
	size_t num_ranges;
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
	size_t num_subexprs;
	Quantifier quantifier;
};

struct Or_t {
	Grammar** subexprs;
	size_t num_subexprs;
	Quantifier quantifier;
};

void grammatica_report_error(GrammaticaContextHandle_t ctx, const char* message);
void grammatica_report_error_with_code(GrammaticaContextHandle_t ctx, GrammaticaErrorCode code, const char* message);
void grammatica_report_notice(GrammaticaContextHandle_t ctx, const char* message);

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

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_INTERNAL_H */
