#ifndef GRAMMATICA_UTILS_H
#define GRAMMATICA_UTILS_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include "grammatica.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GRAMMATICA_HEX_BUFFER_SIZE 8

int char_to_hex(const char* const c, char* out, const size_t size);

// /* String Builder - dynamic string construction utility */
// typedef struct StringBuilder_t StringBuilder;

// /**
//  * Create a new string builder
//  * @param ctx Context handle
//  * @param initial_capacity Initial buffer capacity (0 for default)
//  * @return New string builder or NULL on error
//  */
// StringBuilder* grammatica_sb_create(GrammaticaContextHandle_t ctx, size_t initial_capacity);
// 
// /**
//  * Append a single character to the string builder
//  * @param sb String builder
//  * @param c Character to append
//  * @return true on success, false on error
//  */
// bool grammatica_sb_append_char(StringBuilder* sb, char c);
// 
// /**
//  * Append a null-terminated string to the string builder
//  * @param sb String builder
//  * @param str String to append
//  * @return true on success, false on error
//  */
// bool grammatica_sb_append(StringBuilder* sb, const char* str);
// 
// /**
//  * Append a string with specified length to the string builder
//  * @param sb String builder
//  * @param str String to append
//  * @param len Number of characters to append
//  * @return true on success, false on error
//  */
// bool grammatica_sb_append_n(StringBuilder* sb, const char* str, size_t len);
// 
// /**
//  * Append a formatted string to the string builder
//  * @param sb String builder
//  * @param format Printf-style format string
//  * @param ... Format arguments
//  * @return true on success, false on error
//  */
// bool grammatica_sb_append_format(StringBuilder* sb, const char* format, ...);
// 
// /**
//  * Clear the string builder (reset to empty string)
//  * @param sb String builder
//  */
// void grammatica_sb_clear(StringBuilder* sb);
// 
// /**
//  * Get the current length of the string builder
//  * @param sb String builder
//  * @return Current length
//  */
// size_t grammatica_sb_length(const StringBuilder* sb);
// 
// /**
//  * Get a read-only pointer to the string builder's data
//  * @param sb String builder
//  * @return Pointer to internal string (do not modify or free)
//  */
// const char* grammatica_sb_cstr(const StringBuilder* sb);
// 
// /**
//  * Finalize the string builder and return ownership of the string
//  * The caller is responsible for freeing the returned string
//  * The string builder becomes invalid after this call
//  * @param sb String builder
//  * @return Newly allocated string (must be freed by caller)
//  */
// char* grammatica_sb_finalize(StringBuilder* sb);
// 
// /**
//  * Destroy the string builder and free all memory
//  * @param sb String builder
//  */
// void grammatica_sb_destroy(StringBuilder* sb);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_UTILS_H */
