#include "string_builder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define INITIAL_CAPACITY 64
#define GROWTH_FACTOR 2

GrammaticaStringBuilder* grammaticaStringBuilderCreate(size_t initial_capacity) {
    GrammaticaStringBuilder* sb = (GrammaticaStringBuilder*)malloc(sizeof(GrammaticaStringBuilder));
    if (sb == NULL) {
        return NULL;
    }
    if (initial_capacity == 0) {
        initial_capacity = INITIAL_CAPACITY;
    }
    sb->data = (char*)malloc(initial_capacity);
    if (sb->data == NULL) {
        free(sb);
        return NULL;
    }
    sb->data[0] = '\0';
    sb->length = 0;
    sb->capacity = initial_capacity;
    return sb;
}

void grammaticaStringBuilderDestroy(GrammaticaStringBuilder* sb) {
    if (sb != NULL) {
        if (sb->data != NULL) {
            free(sb->data);
        }
        free(sb);
    }
}

static GrammaticaError_t ensure_capacity(GrammaticaStringBuilder* sb, size_t required) {
    if (sb->capacity >= required) {
        return GRAMMATICA_OK;
    }
    size_t new_capacity = sb->capacity;
    while (new_capacity < required) {
        new_capacity *= GROWTH_FACTOR;
    }
    char* new_data = (char*)realloc(sb->data, new_capacity);
    if (new_data == NULL) {
        return GRAMMATICA_ERROR_OUT_OF_MEMORY;
    }
    sb->data = new_data;
    sb->capacity = new_capacity;
    return GRAMMATICA_OK;
}

GrammaticaError_t grammaticaStringBuilderAppend(GrammaticaStringBuilder* sb, const char* str) {
    if (sb == NULL || str == NULL) {
        return GRAMMATICA_ERROR_NULL_POINTER;
    }
    size_t str_len = strlen(str);
    size_t required = sb->length + str_len + 1;
    GrammaticaError_t err = ensure_capacity(sb, required);
    if (err != GRAMMATICA_OK) {
        return err;
    }
    memcpy(sb->data + sb->length, str, str_len);
    sb->length += str_len;
    sb->data[sb->length] = '\0';
    return GRAMMATICA_OK;
}

GrammaticaError_t grammaticaStringBuilderAppend_char(GrammaticaStringBuilder* sb, char ch) {
    if (sb == NULL) {
        return GRAMMATICA_ERROR_NULL_POINTER;
    }
    size_t required = sb->length + 2;
    GrammaticaError_t err = ensure_capacity(sb, required);
    if (err != GRAMMATICA_OK) {
        return err;
    }
    sb->data[sb->length] = ch;
    sb->length++;
    sb->data[sb->length] = '\0';
    return GRAMMATICA_OK;
}

GrammaticaError_t grammaticaStringBuilderAppend_format(GrammaticaStringBuilder* sb, const char* format, ...) {
    if (sb == NULL || format == NULL) {
        return GRAMMATICA_ERROR_NULL_POINTER;
    }
    va_list args;
    va_start(args, format);
    // First, determine how much space we need
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);
    if (needed < 0) {
        va_end(args);
        return GRAMMATICA_ERROR_INVALID_ARGUMENT;
    }
    size_t required = sb->length + needed + 1;
    GrammaticaError_t err = ensure_capacity(sb, required);
    if (err != GRAMMATICA_OK) {
        va_end(args);
        return err;
    }
    vsnprintf(sb->data + sb->length, needed + 1, format, args);
    sb->length += needed;
    va_end(args);
    return GRAMMATICA_OK;
}

const char* grammaticaStringBuilderGet(const GrammaticaStringBuilder* sb) {
    if (sb == NULL) {
        return NULL;
    }
    return sb->data;
}

size_t grammaticaStringBuilderLength(const GrammaticaStringBuilder* sb) {
    if (sb == NULL) {
        return 0;
    }
    return sb->length;
}

void grammaticaStringBuilderClear(GrammaticaStringBuilder* sb) {
    if (sb != NULL) {
        sb->length = 0;
        if (sb->data != NULL) {
            sb->data[0] = '\0';
        }
    }
}

char* grammaticaStringBuilderExtract(GrammaticaStringBuilder* sb) {
    if (sb == NULL || sb->data == NULL) {
        return NULL;
    }
    char* result = sb->data;
    sb->data = NULL;
    sb->length = 0;
    sb->capacity = 0;
    return result;
}
