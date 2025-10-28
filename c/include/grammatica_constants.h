#ifndef GRAMMATICA_CONSTANTS_H
#define GRAMMATICA_CONSTANTS_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const char* const DIGITS;
extern const char* const ASCII_LETTERS;
extern const char* const PUNCTUATION;
extern const char* const SPACE;

bool char_is_digit(const char c);
bool char_is_ascii_letter(const char c);
bool char_is_punctuation(const char c);
bool char_is_space(const char c);
bool char_is_always_safe(const char c);
bool char_is_string_literal_escape(const char c);
bool char_is_range_escape(const char c);

const char* char_get_escape(const char c);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_CONSTANTS_H */
