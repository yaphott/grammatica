/*
 * Example usage of the refactored utils functions
 * Demonstrates the improved API with proper error handling
 */

#include <stdio.h>
#include "grammatica.h"
#include "grammatica_utils.h"

int main(void) {
    GrammaticaContextHandle_t ctx = grammatica_init();
    if (!ctx) {
        fprintf(stderr, "Failed to initialize context\n");
        return 1;
    }

    /* Example 1: Using the helper macros for stack allocation (cleanest) */
    {
        char buf[GRAMMATICA_CODEPOINT_BUFFER_SIZE];
        GrammaticaError_t err = ord_to_cpoint(ctx, 0x1F600, buf, sizeof(buf));
        
        if (err == GRAMMATICA_ERROR_NONE) {
            printf("Codepoint: %s\n", buf);  // Prints: \U0001F600
        } else {
            fprintf(stderr, "Error: %s\n", grammatica_get_last_error(ctx));
        }
    }

    /* Example 2: Using the macro directly in declaration (most concise) */
    {
        char buf[GRAMMATICA_HEX_BUFFER_SIZE];
        if (char_to_hex(ctx, 'A', buf, sizeof(buf)) == GRAMMATICA_ERROR_NONE) {
            printf("Hex: %s\n", buf);  // Prints: \x41
        }
    }

    /* Example 3: Multiple conversions with clear error handling */
    {
        char cpoint_buf[GRAMMATICA_CODEPOINT_BUFFER_SIZE];
        char hex_buf[GRAMMATICA_HEX_BUFFER_SIZE];
        
        unsigned char c = 'Z';
        
        if (char_to_cpoint(ctx, c, cpoint_buf, sizeof(cpoint_buf)) == GRAMMATICA_ERROR_NONE &&
            char_to_hex(ctx, c, hex_buf, sizeof(hex_buf)) == GRAMMATICA_ERROR_NONE) {
            printf("Character '%c': codepoint=%s, hex=%s\n", c, cpoint_buf, hex_buf);
        }
    }

    /* Example 4: Checking for errors properly */
    {
        char small_buf[4];  // Too small!
        GrammaticaError_t err = ord_to_cpoint(ctx, 0x10000, small_buf, sizeof(small_buf));
        
        if (err != GRAMMATICA_ERROR_NONE) {
            printf("Expected error occurred: %s (code: %d)\n", 
                   grammatica_error_code_to_string(err), err);
        }
    }

    grammatica_finish(ctx);
    return 0;
}
