#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_constants.h"
#include "grammatica_internal.h"
#include "grammatica_utils.h"

/* Helper to escape a character for string literal context */
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

/* Create String */
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

/* Destroy String */
void grammatica_string_destroy(GrammaticaContextHandle_t ctx, String* str) {
	(void)ctx;
	if (!str) {
		return;
	}
	free(str->value);
	free(str);
}

/* Render String */
char* grammatica_string_render(GrammaticaContextHandle_t ctx, const String* str, bool full, bool wrap) {
	(void)full;
	(void)wrap;
	if (!ctx || !str) {
		return NULL;
	}
	if (strlen(str->value) == 0) {
		return NULL;
	}
	/* Build escaped string */
	char* result = (char*)malloc(4096);
	if (!result) {
		grammatica_report_error(ctx, "Memory allocation failed");
		return NULL;
	}
	size_t pos = 0;
	result[pos++] = '"';
	for (const char* p = str->value; *p; p++) {
		char* escaped = escape_char_for_string(*p);
		if (!escaped) {
			free(result);
			return NULL;
		}
		pos += snprintf(result + pos, 4096 - pos, "%s", escaped);
		free(escaped);
	}
	result[pos++] = '"';
	result[pos] = '\0';
	char* final_result = strdup(result);
	free(result);
	return final_result;
}

/* Simplify String */
Grammar* grammatica_string_simplify(GrammaticaContextHandle_t ctx, const String* str) {
	if (!ctx || !str) {
		return NULL;
	}
	if (strlen(str->value) == 0) {
		return NULL;
	}
	String* copy = grammatica_string_copy(ctx, str);
	if (!copy) {
		return NULL;
	}
	Grammar* grammar = (Grammar*)malloc(sizeof(Grammar));
	if (!grammar) {
		grammatica_string_destroy(ctx, copy);
		return NULL;
	}
	grammar->type = GRAMMAR_TYPE_STRING;
	grammar->data = copy;
	return grammar;
}

/* Convert String to string representation */
char* grammatica_string_as_string(GrammaticaContextHandle_t ctx, const String* str) {
	if (!ctx || !str) {
		return NULL;
	}
	char* result = (char*)malloc(4096);
	if (!result) {
		return NULL;
	}
	snprintf(result, 4096, "String(value='%s')", str->value);
	char* final_result = strdup(result);
	free(result);
	return final_result;
}

/* Check if two Strings are equal */
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

/* Copy String */
String* grammatica_string_copy(GrammaticaContextHandle_t ctx, const String* str) {
	if (!ctx || !str) {
		return NULL;
	}
	return grammatica_string_create(ctx, str->value);
}

/* Get string value */
const char* grammatica_string_get_value(GrammaticaContextHandle_t ctx, const String* str) {
	if (!ctx || !str) {
		return NULL;
	}
	return str->value;
}
