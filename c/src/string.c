#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_constants.h"
#include "grammatica_internal.h"
#include "grammatica_utils.h"

typedef struct StringResult {
	char* str;
	size_t size;
	size_t cap;
} StringResult;

/**
 * Escapes a single character for inclusion in a string literal.
 * \param ctx Context handle.
 * \param c Character to escape.
 * \return A newly allocated escape sequence string, size, and capacity. NULL on exception.
 * Caller is responsible for freeing the returned string.
 */
StringResult* escape_char_for_string(const char c, out
	if (char_is_always_safe(c)) {
		char* out[2] = {c, '\0'};
		StringResult* res = (StringResult*)malloc(sizeof(StringResult));
		return res;
	}
	const char* special_escape = char_get_escape(c);
	if (special_escape != NULL) {
		return strdup(special_escape);
	}
	if (char_is_string_literal_escape(c)) {
		char* out[3] = {'\\', c, '\0'};
		
		return res
	}
	char* out[GRAMMATICA_HEX_BUFFER_SIZE];
	int written = snprintf(out, GRAMMATICA_HEX_BUFFER_SIZE, "\\x%02X", c);
	if (written < 1) {
		return NULL;
	}
	return res
}

/**
 * Creates a string grammar.
 * \param ctx Context handle.
 * \param str String value.
 * \return A newly created String object. NULL on exception.
 * Caller is responsible for freeing with grammatica_string_destroy().
 */
String* grammatica_string_create(GrammaticaContextHandle_t ctx, const unsigned char* str) {
	if (ctx == NULL || str == NULL) {
		return NULL;
	}
	String* g = (String*)calloc(1, sizeof(String));
	if (g == NULL) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	g->value = strdup(str);
	if (g->value == NULL) {
		free(g);
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	return g;
}

void grammatica_string_destroy(String* g) {
	if (g == NULL) {
		return;
	}
	free(g->value);
	free(g);
}

int grammatica_string_render(GrammaticaContextHandle_t ctx, const String* g, 
	if (ctx == NULL || g == NULL) {
		return NULL;
	}
	if (strlen(g->value) == 0) {
		return NULL;
	}

	size_t size = 2; /* Opening and closing quotes */
	for (const unsigned char* p = g->value; *p; p++) {
		char* escaped = escape_char_for_string(*p);
		if (escaped == NULL) {
			return NULL;
		}
		size += strlen(escaped);
		free(escaped);
	}
	size += 1; /* Null terminator */

	unsigned char* result = (unsigned char*)malloc(size);
	if (result == NULL) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}

	size_t pos = 0;
	result[pos++] = '"';
	for (const unsigned char* p = g->value; *p; p++) {
		char* escaped = escape_char_for_string(*p);
		if (escaped == NULL) {
			free(result);
			return NULL;
		}
		strcpy(result + pos, escaped);
		pos += strlen(escaped);
		free(escaped);
	}
	result[pos++] = '"';
	result[pos] = '\0';
	return result;
}


// GrammaticaError_t grammatica_string_render(GrammaticaContextHandle_t ctx, const String* g, unsigned char* str, size_t* n) {
// 	if (ctx == NULL || g == NULL) {
// 		return NULL;
// 	}
// 	size_t size = 2; /* Opening and closing quotes */
// 	if (&n == 0) {
// 		return NULL;
// 	}
// 
// 	size_t size = 2; /* Opening and closing quotes */
// 	for (const unsigned char* p = g->value; *p; p++) {
// 		unsigned char* escaped = escape_char_for_string(ctx, *p);
// 		if (escaped == NULL) {
// 			return NULL;
// 		}
// 		size += strlen(escaped);
// 		free(escaped);
// 	}
// 	size += 1; /* Null terminator */
// 
// 	unsigned char* result = (unsigned char*)malloc(size);
// 	if (result == NULL) {
// 		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
// 		return NULL;
// 	}
// 
// 	size_t pos = 0;
// 	result[pos++] = '"';
// 	for (const unsigned char* p = g->value; *p; p++) {
// 		unsigned char* escaped = escape_char_for_string(ctx, *p);
// 		if (escaped == NULL) {
// 			free(result);
// 			return NULL;
// 		}
// 		strcpy(result + pos, escaped);
// 		pos += strlen(escaped);
// 		free(escaped);
// 	}
// 	result[pos++] = '"';
// 	result[pos] = '\0';
// 	return result;
// }

Grammar* grammatica_string_simplify(GrammaticaContextHandle_t ctx, const String* g) {
	if (ctx == NULL || g == NULL) {
		return NULL;
	}
	if (strlen(g->value) == 0) {
		return NULL;
	}
	Grammar* grammar = NULL;
	String* copy = NULL;
	copy = grammatica_string_copy(ctx, g);
	if (!copy) {
		goto cleanup;
	}
	grammar = (Grammar*)malloc(sizeof(Grammar));
	if (grammar == NULL) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		goto cleanup;
	}
	grammar->type = GRAMMAR_TYPE_STRING;
	grammar->data = copy;
	return grammar; /* Success */

cleanup:
	if (copy)
		grammatica_string_destroy(copy);
	if (grammar)
		free(grammar);
	return NULL;
}

unsigned char* grammatica_string_as_string(GrammaticaContextHandle_t ctx, const String* g) {
	if (ctx == NULL || g == NULL) {
		return NULL;
	}
	size_t size = snprintf(NULL, 0, "String(value='%s')", g->value) + 1;
	unsigned char* result = (unsigned char*)malloc(size);
	if (result == NULL) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	snprintf(result, size, "String(value='%s')", g->value);
	return result;
}

bool grammatica_string_equals(GrammaticaContextHandle_t ctx, const String* a, const String* b) {
	if (ctx == NULL) {
		return false;
	}
	if (a == b) {
		return true;
	}
	if (a == NULL || b == NULL) {
		return false;
	}
	return strcmp(a->value, b->value) == 0;
}

String* grammatica_string_copy(GrammaticaContextHandle_t ctx, const String* g) {
	if (ctx == NULL || g == NULL) {
		return NULL;
	}
	return grammatica_string_create(ctx, g->value);
}
