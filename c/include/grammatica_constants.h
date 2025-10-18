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

bool char_is_digit(char c);
bool char_is_ascii_letter(char c);
bool char_is_punctuation(char c);
bool char_is_space(char c);
bool char_is_always_safe(char c);
bool char_is_string_literal_escape(char c);
bool char_is_range_escape(char c);

const char* char_get_escape(char c);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_CONSTANTS_H */
