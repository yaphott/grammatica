#include <string.h>

#include "grammatica_constants.h"

const unsigned char* const DIGITS = "0123456789";
const unsigned char* const ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const unsigned char* const PUNCTUATION = "!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
const unsigned char* const SPACE = " ";

bool char_is_digit(unsigned char c) {
	return (c >= '0' && c <= '9');
}

bool char_is_ascii_letter(unsigned char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool char_is_punctuation(unsigned char c) {
	return c != '\0' && strchr(PUNCTUATION, c) != NULL;
}

bool char_is_space(unsigned char c) {
	return c == ' ';
}

bool char_is_always_safe(unsigned char c) {
	return char_is_digit(c) || char_is_ascii_letter(c) || char_is_punctuation(c) || char_is_space(c);
}

bool char_is_string_literal_escape(unsigned char c) {
	return (c == '"' || c == '\\');
}

bool char_is_range_escape(unsigned char c) {
	return (c == '^' || c == '-' || c == '[' || c == ']' || c == '\\');
}

const unsigned char* char_get_escape(unsigned char c) {
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
