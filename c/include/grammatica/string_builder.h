/**
 * @file string_builder.h
 * @brief String builder for dynamic string construction
 */

#ifndef GRAMMATICA_STRING_BUILDER_H
#define GRAMMATICA_STRING_BUILDER_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief String builder structure
 */
typedef struct grammatica_string_builder_s {
    char* data;
    size_t length;
    size_t capacity;
} GrammaticaStringBuilder;

/**
 * @brief Create a new string builder
 * @param initial_capacity Initial capacity of the string builder
 * @return Pointer to the new string builder, or NULL on error
 */
GrammaticaStringBuilder* grammaticaStringBuilderCreate(size_t initial_capacity);

/**
 * @brief Destroy a string builder
 * @param sb String builder to destroy
 */
void grammaticaStringBuilderDestroy(GrammaticaStringBuilder* sb);

/**
 * @brief Append a string to the string builder
 * @param sb String builder
 * @param str String to append
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError_t grammaticaStringBuilderAppend(GrammaticaStringBuilder* sb, const char* str);

/**
 * @brief Append a character to the string builder
 * @param sb String builder
 * @param ch Character to append
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError_t grammaticaStringBuilderAppend_char(GrammaticaStringBuilder* sb, char ch);

/**
 * @brief Append a formatted string to the string builder
 * @param sb String builder
 * @param format Format string
 * @param ... Format arguments
 * @return GRAMMATICA_OK on success, error code otherwise
 */
GrammaticaError_t grammaticaStringBuilderAppend_format(GrammaticaStringBuilder* sb, const char* format, ...);

/**
 * @brief Get the current string from the string builder
 * @param sb String builder
 * @return Pointer to the string (do not modify or free)
 */
const char* grammaticaStringBuilderGet(const GrammaticaStringBuilder* sb);

/**
 * @brief Get the length of the string in the string builder
 * @param sb String builder
 * @return Length of the string
 */
size_t grammaticaStringBuilderLength(const GrammaticaStringBuilder* sb);

/**
 * @brief Clear the string builder
 * @param sb String builder
 */
void grammaticaStringBuilderClear(GrammaticaStringBuilder* sb);

/**
 * @brief Extract the string from the string builder (caller must free)
 * @param sb String builder
 * @return Pointer to the string (caller must free), or NULL on error
 */
char* grammaticaStringBuilderExtract(GrammaticaStringBuilder* sb);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_STRING_BUILDER_H */
