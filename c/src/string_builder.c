/*
 * String Builder Utility for Grammatica C Library
 * Provides dynamic string building with automatic memory management
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "grammatica_internal.h"
#include "grammatica_utils.h"

#define INITIAL_CAPACITY 256
#define GROWTH_FACTOR 2

typedef struct StringBuilder_t {
	char* data;
	size_t length;
	size_t capacity;
	GrammaticaContextHandle_t ctx;
} StringBuilder;

/**
 * Create a new string builder with the specified initial capacity
 */
StringBuilder* grammatica_sb_create(GrammaticaContextHandle_t ctx, size_t initial_capacity) {
	VALIDATE_CONTEXT_RET_NULL(ctx);
	
	if (initial_capacity == 0) {
		initial_capacity = INITIAL_CAPACITY;
	}
	
	StringBuilder* sb = (StringBuilder*)malloc(sizeof(StringBuilder));
	if (!sb) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, 
			"Failed to allocate string builder");
		return NULL;
	}
	
	sb->data = (char*)malloc(initial_capacity);
	if (!sb->data) {
		free(sb);
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, 
			"Failed to allocate string builder buffer");
		return NULL;
	}
	
	sb->data[0] = '\0';
	sb->length = 0;
	sb->capacity = initial_capacity;
	sb->ctx = ctx;
	
	return sb;
}

/**
 * Ensure the string builder has at least the specified capacity
 */
static bool sb_ensure_capacity(StringBuilder* sb, size_t needed_capacity) {
	if (sb->capacity >= needed_capacity) {
		return true;
	}
	
	size_t new_capacity = sb->capacity;
	while (new_capacity < needed_capacity) {
		new_capacity *= GROWTH_FACTOR;
	}
	
	char* new_data = (char*)realloc(sb->data, new_capacity);
	if (!new_data) {
		grammatica_report_error_with_code(sb->ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY,
			"Failed to grow string builder buffer");
		return false;
	}
	
	sb->data = new_data;
	sb->capacity = new_capacity;
	return true;
}

/**
 * Append a character to the string builder
 */
bool grammatica_sb_append_char(StringBuilder* sb, char c) {
	if (!sb) return false;
	
	if (!sb_ensure_capacity(sb, sb->length + 2)) { /* +2 for char and null terminator */
		return false;
	}
	
	sb->data[sb->length++] = c;
	sb->data[sb->length] = '\0';
	return true;
}

/**
 * Append a string to the string builder
 */
bool grammatica_sb_append(StringBuilder* sb, const char* str) {
	if (!sb || !str) return false;
	
	size_t str_len = strlen(str);
	if (str_len == 0) return true;
	
	if (!sb_ensure_capacity(sb, sb->length + str_len + 1)) {
		return false;
	}
	
	memcpy(sb->data + sb->length, str, str_len);
	sb->length += str_len;
	sb->data[sb->length] = '\0';
	return true;
}

/**
 * Append a string with a specified length to the string builder
 */
bool grammatica_sb_append_n(StringBuilder* sb, const char* str, size_t len) {
	if (!sb || !str || len == 0) return false;
	
	if (!sb_ensure_capacity(sb, sb->length + len + 1)) {
		return false;
	}
	
	memcpy(sb->data + sb->length, str, len);
	sb->length += len;
	sb->data[sb->length] = '\0';
	return true;
}

/**
 * Append a formatted string to the string builder
 */
bool grammatica_sb_append_format(StringBuilder* sb, const char* format, ...) {
	if (!sb || !format) return false;
	
	va_list args, args_copy;
	va_start(args, format);
	va_copy(args_copy, args);
	
	/* Calculate needed size */
	int needed = vsnprintf(NULL, 0, format, args);
	va_end(args);
	
	if (needed < 0) {
		va_end(args_copy);
		return false;
	}
	
	if (!sb_ensure_capacity(sb, sb->length + needed + 1)) {
		va_end(args_copy);
		return false;
	}
	
	/* Actually format the string */
	vsnprintf(sb->data + sb->length, needed + 1, format, args_copy);
	va_end(args_copy);
	
	sb->length += needed;
	return true;
}

/**
 * Clear the string builder (reset to empty string)
 */
void grammatica_sb_clear(StringBuilder* sb) {
	if (!sb) return;
	
	sb->length = 0;
	if (sb->data) {
		sb->data[0] = '\0';
	}
}

/**
 * Get the current length of the string builder
 */
size_t grammatica_sb_length(const StringBuilder* sb) {
	return sb ? sb->length : 0;
}

/**
 * Get a read-only pointer to the string builder's data
 */
const char* grammatica_sb_cstr(const StringBuilder* sb) {
	return sb ? sb->data : "";
}

/**
 * Finalize the string builder and return ownership of the string
 * The caller is responsible for freeing the returned string
 * The string builder becomes invalid after this call
 */
char* grammatica_sb_finalize(StringBuilder* sb) {
	if (!sb) return NULL;
	
	char* result = sb->data;
	sb->data = NULL;
	sb->length = 0;
	sb->capacity = 0;
	free(sb);
	
	return result;
}

/**
 * Destroy the string builder and free all memory
 */
void grammatica_sb_destroy(StringBuilder* sb) {
	if (!sb) return;
	
	if (sb->data) {
		free(sb->data);
	}
	free(sb);
}
