#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica_utils.h"

char* char_to_cpoint(char c) {
	return ord_to_cpoint((unsigned char)c);
}

char* ord_to_cpoint(int ordinal) {
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

char* char_to_hex(char c) {
	return ord_to_hex((unsigned char)c);
}

char* ord_to_hex(int ordinal) {
	char* result = (char*)malloc(16);
	if (result == NULL) {
		return NULL;
	}
	snprintf(result, 16, "\\x%02X", ordinal);
	return result;
}
