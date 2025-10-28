#include <string.h>

#include "grammatica_constants.h"

const unsigned char* const DIGITS = "0123456789";
const unsigned char* const ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const unsigned char* const PUNCTUATION = "!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
const unsigned char* const SPACE = " ";

/**
 * Check if a character is a digit (0-9).
 * \param c Character to check.
 * \return true if digit, otherwise false.
 */
bool char_is_digit(const char c) {
	return (c >= '0' && c <= '9');
}

/**
 * Check if a character is an ASCII letter (a-z, A-Z).
 * \param c Character to check.
 * \return true if ASCII letter, otherwise false.
 */
bool char_is_ascii_letter(const char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * Check if a character is a punctuation character.
 * \param c Character to check.
 * \return true if punctuation, otherwise false.
 */
bool char_is_punctuation(const char c) {
	return c != '\0' && strchr(PUNCTUATION, c) != NULL;
}

/**
 * Check if a character is a space character.
 * \param c Character to check.
 * \return true if space, otherwise false.
 */
bool char_is_space(const char c) {
	return c == ' ';
}

/**
 * Check if a character is always safe (does not need escaping).
 * \param c Character to check.
 * \return true if always safe, otherwise false.
 */
bool char_is_always_safe(const char c) {
	return char_is_digit(c) || char_is_ascii_letter(c) || char_is_punctuation(c) || char_is_space(c);
}

/**
 * Check if a character is a string literal escape character.
 * \param c Character to check.
 * \return true if string literal escape, otherwise false.
 */
bool char_is_string_literal_escape(const char c) {
	return (c == '"' || c == '\\');
}

/**
 * Check if a character is a range escape character.
 * \param c Character to check.
 * \return true if range escape, otherwise false.
 */
bool char_is_range_escape(const char c) {
	return (c == '^' || c == '-' || c == '[' || c == ']' || c == '\\');
}

/**
 * Get the escape sequence for a character if it has one.
 * \param c Character to check.
 * \return Escape sequence string or NULL if none.
 */
const char* char_get_escape(const char c) {
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
