#include "grammatica/common.h"

const char* Grammatica_errorString(GrammaticaError_t error) {
    switch (error) {
        case GRAMMATICA_OK:
            return "Success";
        case GRAMMATICA_ERROR_NULL_POINTER:
            return "Null pointer";
        case GRAMMATICA_ERROR_INVALID_ARGUMENT:
            return "Invalid argument";
        case GRAMMATICA_ERROR_OUT_OF_MEMORY:
            return "Out of memory";
        case GRAMMATICA_ERROR_INVALID_STATE:
            return "Invalid state";
        case GRAMMATICA_ERROR_BUFFER_TOO_SMALL:
            return "Buffer too small";
        default:
            return "Unknown error";
    }
}
