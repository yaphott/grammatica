#include <stdio.h>
#include <string.h>
#include "c/include/grammatica.h"

int main() {
    GrammaticaContextHandle_t ctx = grammatica_init();
    if (ctx == NULL) {
        fprintf(stderr, "Failed to initialize Grammatica context\n");
        return 1;
    }
    
    // Test null byte in String
    GrammaticaStringHandle_t str_with_null = GrammaticaString_create(ctx, "\x00", 1);
    char* rendered = GrammaticaString_render(ctx, str_with_null);
    
    printf("String with null byte: '%s'\n", rendered);
    printf("Expected: '\"\\x00\"'\n");
    
    int success = (strcmp(rendered, "\"\\x00\"") == 0);
    
    GrammaticaString_destroy(ctx, str_with_null);
    grammatica_finish(ctx);
    
    if (success) {
        printf("\n✓ NULL BYTE TEST PASSED\n");
        return 0;
    } else {
        printf("\n✗ NULL BYTE TEST FAILED\n");
        return 1;
    }
}
