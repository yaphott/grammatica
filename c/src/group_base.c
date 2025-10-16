#include "grammatica.h"
#include "grammatica_internal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Helper function to render quantifier */
char* render_quantifier(Quantifier quantifier) {
    int lower = quantifier.lower;
    int upper = quantifier.upper;
    if (lower == 1 && upper == 1) {
        return NULL;
    }
    if (lower == 0) {
        if (upper == -1) {
            return strdup("*");
        }
        if (upper == 1) {
            return strdup("?");
        }
    }
    if (upper == -1) {
        if (lower == 1) {
            return strdup("+");
        }
        char* result = (char*)malloc(32);
        if (result) {
            snprintf(result, 32, "{%d,}", lower);
        }
        return result;
    }
    if (lower == upper) {
        char* result = (char*)malloc(32);
        if (result) {
            snprintf(result, 32, "{%d}", lower);
        }
        return result;
    }
    char* result = (char*)malloc(32);
    if (result) {
        snprintf(result, 32, "{%d,%d}", lower, upper);
    }
    return result;
}

/* Helper function to check if quantifier needs wrapping */
bool quantifier_needs_wrap(Quantifier quantifier) {
    return !(quantifier.lower == 1 && quantifier.upper == 1);
}
