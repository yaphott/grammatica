#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_constants.h"
#include "grammatica_internal.h"
#include "grammatica_utils.h"

/* The caller owns the result */
static char* escape_char_for_string(char c) {
	if (char_is_always_safe(c)) {
		char* result = (char*)malloc(2);
		if (result == NULL) {
			return NULL;
		}
		result[0] = c;
		result[1] = '\0';
		return result;
	}
	const char* special_escape = char_get_escape(c);
	if (special_escape != NULL) {
		return strdup(special_escape);
	}
	if (char_is_string_literal_escape(c)) {
		char* result = (char*)malloc(3);
		if (result == NULL) {
			return NULL;
		}
		result[0] = '\\';
		result[1] = c;
		result[2] = '\0';
		return result;
	}
	return char_to_hex(c);
}

String* grammatica_string_create(GrammaticaContextHandle_t ctx, const char* value) {
	if (ctx == NULL || value == NULL) {
		return NULL;
	}
	String* str = (String*)calloc(1, sizeof(String));
	if (str == NULL) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	str->value = strdup(value);
	if (str->value == NULL) {
		free(str);
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	return str;
}

void grammatica_string_destroy(GrammaticaContextHandle_t ctx, String* str) {
	(void)ctx;
	if (str == NULL) {
		return;
	}
	free(str->value);
	free(str);
}

char* grammatica_string_render(GrammaticaContextHandle_t ctx, const String* str) {
	if (ctx == NULL || str == NULL) {
		return NULL;
	}
	if (strlen(str->value) == 0) {
		return NULL;
	}
	
	/* First pass: calculate needed size */
	size_t needed = 2; /* Opening and closing quotes */
	for (const char* p = str->value; *p; p++) {
		char* escaped = escape_char_for_string(*p);
		if (escaped == NULL) {
			return NULL;
		}
		needed += strlen(escaped);
		free(escaped);
	}
	needed += 1; /* Null terminator */
	
	/* Allocate exactly what we need */
	char* result = (char*)malloc(needed);
	if (result == NULL) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	
	/* Second pass: build the string */
	size_t pos = 0;
	result[pos++] = '"';
	for (const char* p = str->value; *p; p++) {
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

Grammar* grammatica_string_simplify(GrammaticaContextHandle_t ctx, const String* str) {
	if (!ctx || !str) {
		return NULL;
	}
	if (strlen(str->value) == 0) {
		return NULL;
	}
	Grammar* grammar = NULL;
	String* copy = NULL;
	copy = grammatica_string_copy(ctx, str);
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
		grammatica_string_destroy(ctx, copy);
	if (grammar)
		free(grammar);
	return NULL;
}

char* grammatica_string_as_string(GrammaticaContextHandle_t ctx, const String* str) {
	if (!ctx || !str) {
		return NULL;
	}
	/* Allocate only what we need */
	size_t needed = snprintf(NULL, 0, "String(value='%s')", str->value) + 1;
	char* result = (char*)malloc(needed);
	if (!result) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	snprintf(result, needed, "String(value='%s')", str->value);
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

String* grammatica_string_copy(GrammaticaContextHandle_t ctx, const String* str) {
	if (ctx == NULL || str == NULL) {
		return NULL;
	}
	return grammatica_string_create(ctx, str->value);
}

const char* grammatica_string_get_value(GrammaticaContextHandle_t ctx, const String* str) {
	if (ctx == NULL || str == NULL) {
		return NULL;
	}
	return str->value;
}
