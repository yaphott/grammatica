// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// 
// #include "grammatica_utils.h"
// 
// /**
//  * Convert a character to its hexadecimal escape representation.
//  * \param c Character to convert.
//  * \param out Output buffer.
//  * \param n Size of the output buffer.
//  * \return Number of characters written (excluding null terminator). Negative on exception.
//  * Caller is responsible for freeing.
//  */
// int char_to_hex(const char const c, char* out, const const size_t n) {
// 	if (n < GRAMMATICA_HEX_BUFFER_SIZE) {
// 		return -1;
// 	}
// 	int written = snprintf(out, GRAMMATICA_HEX_BUFFER_SIZE, "\\x%02X", c);
// 	if (written < 1) {
// 		return -1;
// 	}
// 	return written;
// }
