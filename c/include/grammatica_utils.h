#ifndef GRAMMATICA_UTILS_H
#define GRAMMATICA_UTILS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Convert character to Unicode code point string */
char* char_to_cpoint(char c);

/* Convert ordinal to Unicode code point string */
char* ord_to_cpoint(int ordinal);

/* Convert character to hex string */
char* char_to_hex(char c);

/* Convert ordinal to hex string */
char* ord_to_hex(int ordinal);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_UTILS_H */
