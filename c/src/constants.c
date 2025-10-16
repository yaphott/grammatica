#include <string.h>

#include "grammatica_constants.h"

/* Character set constants */
const char* const DIGITS = "0123456789";
const char* const ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* const PUNCTUATION = "!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
const char* const SPACE = " ";

/* Check if character is a digit */
bool char_is_digit(char c) {
	return (c >= '0' && c <= '9');
}

/* Check if character is an ASCII letter */
bool char_is_ascii_letter(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/* Check if character is punctuation */
bool char_is_punctuation(char c) {
	return strchr(PUNCTUATION, c) != NULL;
}

/* Check if character is space */
bool char_is_space(char c) {
	return c == ' ';
}

/* Check if character is always safe (no escaping needed in most contexts) */
bool char_is_always_safe(char c) {
	return char_is_digit(c) || char_is_ascii_letter(c) || char_is_punctuation(c) || char_is_space(c);
}

/* Check if character needs escaping in string literals */
bool char_is_string_literal_escape(char c) {
	return (c == '"' || c == '\\');
}

/* Check if character needs escaping in character ranges */
bool char_is_range_escape(char c) {
	return (c == '^' || c == '-' || c == '[' || c == ']' || c == '\\');
}

/* Get escape sequence for special characters (\n, \r, \t) */
const char* char_get_escape(char c) {
	switch (c) {
		case '\n':
			return "\\n";
		case '\r':
			return "\\r";
		case '\t':
			return "\\t";
		default:
			return NULL;
	}
}
