#include <grammatica.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Grammatica C Example\n");
    printf("====================\n\n");
    
    // Example 1: Simple string
    printf("Example 1: Simple String\n");
    GrammaticaString* hello = grammaticaStringCreate("Hello, World!");
    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)hello, true, true);
    if (rendered != NULL) {
        printf("Rendered: %s\n\n", rendered);
        free(rendered);
    }
    grammaticaGrammarUnref((GrammaticaGrammar*)hello);
    
    // Example 2: Character range
    printf("Example 2: Character Range\n");
    GrammaticaCharRangeEntry range = {'a', 'z'};
    GrammaticaCharRange* lowercase = grammaticaCharRangeCreate(&range, 1, false);
    rendered = grammaticaGrammarRender((GrammaticaGrammar*)lowercase, true, true);
    if (rendered != NULL) {
        printf("Rendered: %s\n\n", rendered);
        free(rendered);
    }
    grammaticaGrammarUnref((GrammaticaGrammar*)lowercase);
    
    // Example 3: Grammar (AND) with multiple strings
    printf("Example 3: Grammar (AND)\n");
    GrammaticaGrammarArray* subexprs = grammaticaGrammarArrayCreate(0);
    GrammaticaString* word1 = grammaticaStringCreate("Hello");
    GrammaticaString* word2 = grammaticaStringCreate(" ");
    GrammaticaString* word3 = grammaticaStringCreate("World");
    
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)word1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)word2);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)word3);
    
    GrammaticaGroupedGrammar* grammar = grammaticaGrammarCreate(subexprs, (GrammaticaQuantifier){1, 1});
    rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    if (rendered != NULL) {
        printf("Rendered: %s\n\n", rendered);
        free(rendered);
    }
    
    // Simplify to merge adjacent strings
    printf("Example 3b: Simplified Grammar\n");
    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    if (simplified != NULL) {
        rendered = grammaticaGrammarRender(simplified, true, true);
        if (rendered != NULL) {
            printf("Rendered: %s\n\n", rendered);
            free(rendered);
        }
        grammaticaGrammarUnref(simplified);
    }
    
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammaticaGrammarUnref((GrammaticaGrammar*)word1);
    grammaticaGrammarUnref((GrammaticaGrammar*)word2);
    grammaticaGrammarUnref((GrammaticaGrammar*)word3);
    grammaticaGrammarArrayDestroy(subexprs);
    
    // Example 4: Or grammar
    printf("Example 4: Or Grammar\n");
    GrammaticaGrammarArray* or_subexprs = grammaticaGrammarArrayCreate(0);
    GrammaticaString* opt1 = grammaticaStringCreate("yes");
    GrammaticaString* opt2 = grammaticaStringCreate("no");
    
    grammaticaGrammarArrayAppend(or_subexprs, (GrammaticaGrammar*)opt1);
    grammaticaGrammarArrayAppend(or_subexprs, (GrammaticaGrammar*)opt2);
    
    GrammaticaGroupedGrammar* or_grammar = grammaticaOrCreate(or_subexprs, (GrammaticaQuantifier){1, 1});
    rendered = grammaticaGrammarRender((GrammaticaGrammar*)or_grammar, true, true);
    if (rendered != NULL) {
        printf("Rendered: %s\n\n", rendered);
        free(rendered);
    }
    
    grammaticaGrammarUnref((GrammaticaGrammar*)or_grammar);
    grammaticaGrammarUnref((GrammaticaGrammar*)opt1);
    grammaticaGrammarUnref((GrammaticaGrammar*)opt2);
    grammaticaGrammarArrayDestroy(or_subexprs);
    
    // Example 5: Derivation rule
    printf("Example 5: Derivation Rule\n");
    GrammaticaString* identifier_value = grammaticaStringCreate("abc");
    GrammaticaDerivationRule* rule = grammaticaDerivationRuleCreate("identifier", (GrammaticaGrammar*)identifier_value);
    rendered = grammaticaGrammarRender((GrammaticaGrammar*)rule, true, true);
    if (rendered != NULL) {
        printf("Rendered: %s\n\n", rendered);
        free(rendered);
    }
    
    grammaticaGrammarUnref((GrammaticaGrammar*)rule);
    grammaticaGrammarUnref((GrammaticaGrammar*)identifier_value);
    
    // Example 6: Quantifiers
    printf("Example 6: Quantifiers\n");
    GrammaticaString* repeated = grammaticaStringCreate("a");
    
    GrammaticaGrammarArray* quant_subexprs = grammaticaGrammarArrayCreate(0);
    grammaticaGrammarArrayAppend(quant_subexprs, (GrammaticaGrammar*)repeated);
    
    // Zero or more (*)
    GrammaticaGroupedGrammar* zero_or_more = grammaticaGrammarCreate(quant_subexprs, (GrammaticaQuantifier){0, 0});
    rendered = grammaticaGrammarRender((GrammaticaGrammar*)zero_or_more, true, true);
    if (rendered != NULL) {
        printf("Zero or more: %s\n", rendered);
        free(rendered);
    }
    grammaticaGrammarUnref((GrammaticaGrammar*)zero_or_more);
    
    // One or more (+)
    GrammaticaGroupedGrammar* one_or_more = grammaticaGrammarCreate(quant_subexprs, (GrammaticaQuantifier){1, 0});
    rendered = grammaticaGrammarRender((GrammaticaGrammar*)one_or_more, true, true);
    if (rendered != NULL) {
        printf("One or more: %s\n", rendered);
        free(rendered);
    }
    grammaticaGrammarUnref((GrammaticaGrammar*)one_or_more);
    
    // Optional (?)
    GrammaticaGroupedGrammar* optional = grammaticaGrammarCreate(quant_subexprs, (GrammaticaQuantifier){0, 1});
    rendered = grammaticaGrammarRender((GrammaticaGrammar*)optional, true, true);
    if (rendered != NULL) {
        printf("Optional: %s\n", rendered);
        free(rendered);
    }
    grammaticaGrammarUnref((GrammaticaGrammar*)optional);
    
    grammaticaGrammarUnref((GrammaticaGrammar*)repeated);
    grammaticaGrammarArrayDestroy(quant_subexprs);
    
    printf("\nAll examples completed successfully!\n");
    
    return 0;
}
