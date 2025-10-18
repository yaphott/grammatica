#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_constants.h"
#include "grammatica_internal.h"
#include "grammatica_utils.h"

static char* escape_char_for_string(char c) {
	if (char_is_always_safe(c)) {
		char* result = (char*)malloc(2);
		if (result) {
			result[0] = c;
			result[1] = '\0';
		}
		return result;
	}
	const char* special_escape = char_get_escape(c);
	if (special_escape) {
		return strdup(special_escape);
	}
	if (char_is_string_literal_escape(c)) {
		char* result = (char*)malloc(3);
		if (result) {
			result[0] = '\\';
			result[1] = c;
			result[2] = '\0';
		}
		return result;
	}
	return char_to_hex(c);
}

String* grammatica_string_create(GrammaticaContextHandle_t ctx, const char* value) {
	if (!ctx || !value) {
		return NULL;
	}
	String* str = (String*)calloc(1, sizeof(String));
	if (!str) {
		grammatica_report_error(ctx, "Memory allocation failed");
		return NULL;
	}
	str->value = strdup(value);
	if (!str->value) {
		free(str);
		grammatica_report_error(ctx, "Memory allocation failed");
		return NULL;
	}
	return str;
}

void grammatica_string_destroy(GrammaticaContextHandle_t ctx, String* str) {
	(void)ctx;
	if (!str) {
		return;
	}
	free(str->value);
	free(str);
}

char* grammatica_string_render(GrammaticaContextHandle_t ctx, const String* str, bool full, bool wrap) {
	(void)full;
	(void)wrap;
	if (!ctx || !str) {
		return NULL;
	}
	if (strlen(str->value) == 0) {
		return NULL;
	}
	/* Build escaped string - use stack buffer for small strings */
	char stack_buf[512];
	char* result = stack_buf;
	size_t buf_size = sizeof(stack_buf);
	size_t pos = 0;

	result[pos++] = '"';
	for (const char* p = str->value; *p; p++) {
		/* Check if we need more space */
		if (pos + 16 > buf_size) {
			if (result == stack_buf) {
				result = (char*)malloc(4096);
				if (!result) {
					grammatica_report_error(ctx, "Memory allocation failed");
					return NULL;
				}
				memcpy(result, stack_buf, pos);
				buf_size = 4096;
			}
		}

		char* escaped = escape_char_for_string(*p);
		if (!escaped) {
			if (result != stack_buf)
				free(result);
			return NULL;
		}
		pos += snprintf(result + pos, buf_size - pos, "%s", escaped);
		free(escaped);
	}
	result[pos++] = '"';
	result[pos] = '\0';

	/* Return final result - if on stack, duplicate it */
	char* final_result = (result == stack_buf) ? strdup(result) : result;
	return final_result;
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
	if (!grammar) {
		grammatica_report_error(ctx, "Memory allocation failed");
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
		grammatica_report_error(ctx, "Memory allocation failed");
		return NULL;
	}
	snprintf(result, needed, "String(value='%s')", str->value);
	return result;
}

bool grammatica_string_equals(GrammaticaContextHandle_t ctx, const String* a, const String* b) {
	if (!ctx) {
		return false;
	}
	if (a == b) {
		return true;
	}
	if (!a || !b) {
		return false;
	}
	return strcmp(a->value, b->value) == 0;
}

String* grammatica_string_copy(GrammaticaContextHandle_t ctx, const String* str) {
	if (!ctx || !str) {
		return NULL;
	}
	return grammatica_string_create(ctx, str->value);
}

const char* grammatica_string_get_value(GrammaticaContextHandle_t ctx, const String* str) {
	if (!ctx || !str) {
		return NULL;
	}
	return str->value;
}
