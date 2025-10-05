/**
 * @file common.h
 * @brief Common definitions and types for the Grammatica library
 */

#ifndef GRAMMATICA_COMMON_H
#define GRAMMATICA_COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Version information
 */
#define GRAMMATICA_VERSION_MAJOR 0
#define GRAMMATICA_VERSION_MINOR 0
#define GRAMMATICA_VERSION_PATCH 0
#define GRAMMATICA_VERSION "0.0.0"

/**
 * @brief Error codes
 */
typedef enum {
    GRAMMATICA_OK = 0,
    GRAMMATICA_ERROR_NULL_POINTER = -1,
    GRAMMATICA_ERROR_INVALID_ARGUMENT = -2,
    GRAMMATICA_ERROR_OUT_OF_MEMORY = -3,
    GRAMMATICA_ERROR_INVALID_STATE = -4,
    GRAMMATICA_ERROR_BUFFER_TOO_SMALL = -5,
} GrammaticaError;

/**
 * @brief Get error message for an error code
 * @param error Error code
 * @return Error message string (static, do not free)
 */
const char* Grammatica_errorString(GrammaticaError error);

#ifdef __cplusplus
}
#endif

#endif /* GRAMMATICA_COMMON_H */
