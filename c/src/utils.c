#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica_utils.h"

/* The caller owns the result */
unsigned char* char_to_cpoint(const unsigned char c) {
	return ord_to_cpoint(c);
}

/* The caller owns the result */
unsigned char* ord_to_cpoint(const int ordinal) {
	char* result = (char*)malloc(16);
	if (result == NULL) {
		return NULL;
	}
	if (ordinal < 0x10000) {
		snprintf(result, 16, "\\u%04X", ordinal);
	} else {
		snprintf(result, 16, "\\U%08X", ordinal);
	}
	return result;
}

/* The caller owns the result */
unsigned char* char_to_hex(const unsigned char c) {
	return ord_to_hex(c);
}

/* The caller owns the result */
unsigned char* ord_to_hex(const int ordinal) {
	unsigned char* result = (char*)malloc(16);
	if (result == NULL) {
		return NULL;
	}
	snprintf(result, 16, "\\x%02X", ordinal);
	return result;
}
