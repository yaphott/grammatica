#ifndef GRAMMATICA_UTILS_H
#define GRAMMATICA_UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include "grammatica.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Character conversion utilities */
char* char_to_cpoint(char c);
char* ord_to_cpoint(int ordinal);
char* char_to_hex(char c);
char* ord_to_hex(int ordinal);

/* String Builder - dynamic string construction utility */
typedef struct StringBuilder_t StringBuilder;

/**
 * Create a new string builder
 * @param ctx Context handle
 * @param initial_capacity Initial buffer capacity (0 for default)
 * @return New string builder or NULL on error
 */
StringBuilder* grammatica_sb_create(GrammaticaContextHandle_t ctx, size_t initial_capacity);

/**
 * Append a single character to the string builder
 * @param sb String builder
 * @param c Character to append
 * @return true on success, false on error
 */
bool grammatica_sb_append_char(StringBuilder* sb, char c);

/**
 * Append a null-terminated string to the string builder
 * @param sb String builder
 * @param str String to append
 * @return true on success, false on error
 */
bool grammatica_sb_append(StringBuilder* sb, const char* str);

/**
 * Append a string with specified length to the string builder
 * @param sb String builder
 * @param str String to append
 * @param len Number of characters to append
 * @return true on success, false on error
 */
bool grammatica_sb_append_n(StringBuilder* sb, const char* str, size_t len);

/**
 * Append a formatted string to the string builder
 * @param sb String builder
 * @param format Printf-style format string
 * @param ... Format arguments
 * @return true on success, false on error
 */
bool grammatica_sb_append_format(StringBuilder* sb, const char* format, ...);

/**
 * Clear the string builder (reset to empty string)
 * @param sb String builder
 */
void grammatica_sb_clear(StringBuilder* sb);

/**
 * Get the current length of the string builder
 * @param sb String builder
 * @return Current length
 */
size_t grammatica_sb_length(const StringBuilder* sb);

/**
 * Get a read-only pointer to the string builder's data
 * @param sb String builder
 * @return Pointer to internal string (do not modify or free)
 */
const char* grammatica_sb_cstr(const StringBuilder* sb);

/**
 * Finalize the string builder and return ownership of the string
 * The caller is responsible for freeing the returned string
 * The string builder becomes invalid after this call
 * @param sb String builder
 * @return Newly allocated string (must be freed by caller)
 */
char* grammatica_sb_finalize(StringBuilder* sb);

/**
 * Destroy the string builder and free all memory
 * @param sb String builder
 */
void grammatica_sb_destroy(StringBuilder* sb);

/* Arena Allocator - bulk temporary allocations */
typedef struct Arena Arena;

/**
 * Create a new arena allocator
 * @param ctx Context handle for error reporting
 * @param block_size Size of each memory block (0 for default 64KB)
 * @return New arena or NULL on error
 */
Arena* arena_create(GrammaticaContextHandle_t ctx, size_t block_size);

/**
 * Allocate memory from the arena
 * @param arena Arena allocator
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 * @note Memory is aligned to 8 bytes
 * @note Individual allocations cannot be freed; use arena_reset() or arena_destroy()
 */
void* arena_alloc(Arena* arena, size_t size);

/**
 * Allocate zero-initialized memory from the arena
 * @param arena Arena allocator
 * @param count Number of elements
 * @param size Size of each element
 * @return Pointer to zero-initialized memory, or NULL on failure
 */
void* arena_calloc(Arena* arena, size_t count, size_t size);

/**
 * Duplicate a string in the arena
 * @param arena Arena allocator
 * @param str String to duplicate
 * @return Pointer to duplicated string, or NULL on failure
 */
char* arena_strdup(Arena* arena, const char* str);

/**
 * Duplicate n characters of a string in the arena
 * @param arena Arena allocator
 * @param str String to duplicate
 * @param n Maximum number of characters to copy
 * @return Pointer to null-terminated duplicated string, or NULL on failure
 */
char* arena_strndup(Arena* arena, const char* str, size_t n);

/**
 * Reset the arena, freeing all allocations
 * @param arena Arena to reset
 * @note Keeps the first block allocated for reuse
 */
void arena_reset(Arena* arena);

/**
 * Get the total bytes allocated by the arena
 * @param arena Arena allocator
 * @return Total bytes allocated across all blocks
 */
size_t arena_total_allocated(const Arena* arena);

/**
 * Get the number of blocks in the arena
 * @param arena Arena allocator
 * @return Number of blocks
 */
size_t arena_block_count(const Arena* arena);

/**
 * Destroy the arena and free all memory
 * @param arena Arena to destroy
 */
void arena_destroy(Arena* arena);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_UTILS_H */
