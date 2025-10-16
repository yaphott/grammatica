#ifndef GRAMMATICA_INTERNAL_H
#define GRAMMATICA_INTERNAL_H

#include "grammatica.h"
#include <pthread.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Internal context structure */
typedef struct GrammaticaContext_t {
    GrammaticaErrorHandler error_handler;
    void* error_userdata;
    GrammaticaNoticeHandler notice_handler;
    void* notice_userdata;
    char error_buffer[1024];
    pthread_mutex_t mutex;
    int initialized;
} GrammaticaContext;

/* Internal grammar structure */
struct Grammar_t {
    GrammarType type;
    void* data;
};

/* Internal CharRange structure */
struct CharRange_t {
    CharRangePair* ranges;
    size_t num_ranges;
    bool negate;
};

/* Internal String structure */
struct String_t {
    char* value;
};

/* Internal DerivationRule structure */
struct DerivationRule_t {
    char* symbol;
    Grammar* value;
};

/* Internal And structure */
struct And_t {
    Grammar** subexprs;
    size_t num_subexprs;
    Quantifier quantifier;
};

/* Internal Or structure */
struct Or_t {
    Grammar** subexprs;
    size_t num_subexprs;
    Quantifier quantifier;
};

/* Helper functions for error reporting */
void grammatica_report_error(GrammaticaContextHandle_t ctx, const char* message);
void grammatica_report_notice(GrammaticaContextHandle_t ctx, const char* message);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_INTERNAL_H */
