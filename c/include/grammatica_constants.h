#ifndef GRAMMATICA_CONSTANTS_H
#define GRAMMATICA_CONSTANTS_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const unsigned char* const DIGITS;
extern const unsigned char* const ASCII_LETTERS;
extern const unsigned char* const PUNCTUATION;
extern const unsigned char* const SPACE;

bool char_is_digit(unsigned char c);
bool char_is_ascii_letter(unsigned char c);
bool char_is_punctuation(unsigned char c);
bool char_is_space(unsigned char c);
bool char_is_always_safe(unsigned char c);
bool char_is_string_literal_escape(unsigned char c);
bool char_is_range_escape(unsigned char c);

const unsigned char* char_get_escape(unsigned char c);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_CONSTANTS_H */
