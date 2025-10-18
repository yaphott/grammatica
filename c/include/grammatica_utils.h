#ifndef GRAMMATICA_UTILS_H
#define GRAMMATICA_UTILS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

char* char_to_cpoint(char c);
char* ord_to_cpoint(int ordinal);
char* char_to_hex(char c);
char* ord_to_hex(int ordinal);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_UTILS_H */
