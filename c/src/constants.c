#include <string.h>

#include "grammatica_constants.h"

const char* const DIGITS = "0123456789";
const char* const ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* const PUNCTUATION = "!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
const char* const SPACE = " ";

bool char_is_digit(char c) {
	return (c >= '0' && c <= '9');
}

bool char_is_ascii_letter(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool char_is_punctuation(char c) {
	return c != '\0' && strchr(PUNCTUATION, c) != NULL;
}

bool char_is_space(char c) {
	return c == ' ';
}

bool char_is_always_safe(char c) {
	return char_is_digit(c) || char_is_ascii_letter(c) || char_is_punctuation(c) || char_is_space(c);
}

bool char_is_string_literal_escape(char c) {
	return (c == '"' || c == '\\');
}

bool char_is_range_escape(char c) {
	return (c == '^' || c == '-' || c == '[' || c == ']' || c == '\\');
}

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
