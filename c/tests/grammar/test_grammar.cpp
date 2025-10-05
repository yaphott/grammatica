#include <gtest/gtest.h>
#include <grammatica.h>

class GrammarTest : public ::testing::Test {
protected:
    GrammaticaGroupedGrammar* grammar = nullptr;
    GrammaticaGrammarArray* subexprs = nullptr;
    void SetUp() override {
        subexprs = grammaticaGrammarArrayCreate(0);
        ASSERT_NE(nullptr, subexprs);
    }
    void TearDown() override {
        if (grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
        }
        if (subexprs != nullptr) {
            grammaticaGrammarArrayDestroy(subexprs);
        }
    }
};

TEST_F(GrammarTest, CreateGrammar) {
    GrammaticaString* str = grammaticaStringCreate("test");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str);

    grammar = grammaticaGrammarCreate(subexprs, {1, 1});
    ASSERT_NE(nullptr, grammar);

    EXPECT_EQ(GRAMMATICA_TYPE_GRAMMAR, grammar->base.type);

    grammaticaGrammarUnref((GrammaticaGrammar*)str);
}

TEST_F(GrammarTest, CreateOr) {
    GrammaticaString* str = grammaticaStringCreate("test");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str);

    grammar = grammaticaOrCreate(subexprs, {1, 1});
    ASSERT_NE(nullptr, grammar);

    EXPECT_EQ(GRAMMATICA_TYPE_OR, grammar->base.type);

    grammaticaGrammarUnref((GrammaticaGrammar*)str);
}

TEST_F(GrammarTest, RenderGrammarSimple) {
    GrammaticaString* str1 = grammaticaStringCreate("Hello");
    GrammaticaString* str2 = grammaticaStringCreate("World");

    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str2);

    grammar = grammaticaGrammarCreate(subexprs, {1, 1});

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("\"Hello\" \"World\"", rendered);
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
}

TEST_F(GrammarTest, RenderOrSimple) {
    GrammaticaString* str1 = grammaticaStringCreate("Hello");
    GrammaticaString* str2 = grammaticaStringCreate("World");

    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str2);

    grammar = grammaticaOrCreate(subexprs, {1, 1});

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("(\"Hello\" | \"World\")", rendered);  // Or is always wrapped
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
}

TEST_F(GrammarTest, RenderWithQuantifier) {
    GrammaticaString* str = grammaticaStringCreate("test");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str);

    grammar = grammaticaGrammarCreate(subexprs, {0, 0});  // *

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("\"test\"*", rendered);
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str);
}

TEST_F(GrammarTest, SimplifyUnwrapSingle) {
    GrammaticaString* str = grammaticaStringCreate("test");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str);

    grammar = grammaticaGrammarCreate(subexprs, {1, 1});

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    // Should unwrap to just the string
    EXPECT_EQ(GRAMMATICA_TYPE_STRING, simplified->type);

    grammaticaGrammarUnref(simplified);
    grammaticaGrammarUnref((GrammaticaGrammar*)str);
}

TEST_F(GrammarTest, SimplifyMergeStrings) {
    GrammaticaString* str1 = grammaticaStringCreate("Hello");
    GrammaticaString* str2 = grammaticaStringCreate(" ");
    GrammaticaString* str3 = grammaticaStringCreate("World");

    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str2);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str3);

    grammar = grammaticaGrammarCreate(subexprs, {1, 1});

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    // Should merge to a single string and unwrap
    EXPECT_EQ(GRAMMATICA_TYPE_STRING, simplified->type);

    GrammaticaString* simplified_str = (GrammaticaString*)simplified;
    EXPECT_STREQ("Hello World", grammaticaStringGetValue(simplified_str));

    grammaticaGrammarUnref(simplified);
    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
    grammaticaGrammarUnref((GrammaticaGrammar*)str3);
}

TEST_F(GrammarTest, OrRemoveDuplicates) {
    GrammaticaString* str1 = grammaticaStringCreate("test");
    GrammaticaString* str2 = grammaticaStringCreate("test");
    GrammaticaString* str3 = grammaticaStringCreate("other");

    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str2);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str3);

    grammar = grammaticaOrCreate(subexprs, {1, 1});

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    EXPECT_EQ(GRAMMATICA_TYPE_OR, simplified->type);

    GrammaticaGroupedGrammar* simplified_or = (GrammaticaGroupedGrammar*)simplified;
    const GrammaticaGrammarArray* simplified_subexprs = grammaticaGroupedGetSubexprs(simplified_or);

    // Should have only 2 items after removing duplicate
    EXPECT_EQ(2, simplified_subexprs->count);

    grammaticaGrammarUnref(simplified);
    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
    grammaticaGrammarUnref((GrammaticaGrammar*)str3);
}

TEST_F(GrammarTest, Equals) {
    GrammaticaString* str1 = grammaticaStringCreate("test");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str1);

    grammar = grammaticaGrammarCreate(subexprs, {1, 1});

    // Create identical grammar
    GrammaticaGrammarArray* other_subexprs = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str2 = grammaticaStringCreate("test");
    grammaticaGrammarArrayAppend(other_subexprs, (GrammaticaGrammar*)str2);
    GrammaticaGroupedGrammar* other = grammaticaGrammarCreate(other_subexprs, {1, 1});

    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar, (GrammaticaGrammar*)other, true));

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
    grammaticaGrammarUnref((GrammaticaGrammar*)other);
    grammaticaGrammarArrayDestroy(other_subexprs);
}

// Additional complex simplification tests
TEST_F(GrammarTest, SimplifyEmpty) {
    grammar = grammaticaGrammarCreate(subexprs, {1, 1});
    
    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    EXPECT_EQ(nullptr, simplified);
}

TEST_F(GrammarTest, SimplifyEmptyOr) {
    grammar = grammaticaOrCreate(subexprs, {1, 1});
    
    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    EXPECT_EQ(nullptr, simplified);
}

TEST_F(GrammarTest, SimplifyNestedGrammar) {
    // Create nested Grammar: Grammar([Grammar([String("a"), String("b")]), Grammar([String("c"), String("d")])])
    GrammaticaGrammarArray* inner1 = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str_a = grammaticaStringCreate("a");
    GrammaticaString* str_b = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(inner1, (GrammaticaGrammar*)str_a);
    grammaticaGrammarArrayAppend(inner1, (GrammaticaGrammar*)str_b);
    GrammaticaGroupedGrammar* inner_grammar1 = grammaticaGrammarCreate(inner1, {1, 1});

    GrammaticaGrammarArray* inner2 = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str_c = grammaticaStringCreate("c");
    GrammaticaString* str_d = grammaticaStringCreate("d");
    grammaticaGrammarArrayAppend(inner2, (GrammaticaGrammar*)str_c);
    grammaticaGrammarArrayAppend(inner2, (GrammaticaGrammar*)str_d);
    GrammaticaGroupedGrammar* inner_grammar2 = grammaticaGrammarCreate(inner2, {1, 1});

    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)inner_grammar1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)inner_grammar2);
    grammar = grammaticaGrammarCreate(subexprs, {1, 1});

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    // Should merge to a single string "abcd"
    EXPECT_EQ(GRAMMATICA_TYPE_STRING, simplified->type);
    EXPECT_STREQ("abcd", grammaticaStringGetValue((GrammaticaString*)simplified));

    grammaticaGrammarUnref(simplified);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_a);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_b);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_c);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_d);
    grammaticaGrammarUnref((GrammaticaGrammar*)inner_grammar1);
    grammaticaGrammarUnref((GrammaticaGrammar*)inner_grammar2);
    grammaticaGrammarArrayDestroy(inner1);
    grammaticaGrammarArrayDestroy(inner2);
}

TEST_F(GrammarTest, SimplifyNestedOr) {
    // Create: Or([Or([String("a"), String("b")]), Or([String("c"), String("d")])])
    GrammaticaGrammarArray* inner1 = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str_a = grammaticaStringCreate("a");
    GrammaticaString* str_b = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(inner1, (GrammaticaGrammar*)str_a);
    grammaticaGrammarArrayAppend(inner1, (GrammaticaGrammar*)str_b);
    GrammaticaGroupedGrammar* inner_or1 = grammaticaOrCreate(inner1, {1, 1});

    GrammaticaGrammarArray* inner2 = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str_c = grammaticaStringCreate("c");
    GrammaticaString* str_d = grammaticaStringCreate("d");
    grammaticaGrammarArrayAppend(inner2, (GrammaticaGrammar*)str_c);
    grammaticaGrammarArrayAppend(inner2, (GrammaticaGrammar*)str_d);
    GrammaticaGroupedGrammar* inner_or2 = grammaticaOrCreate(inner2, {1, 1});

    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)inner_or1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)inner_or2);
    grammar = grammaticaOrCreate(subexprs, {1, 1});

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    // Should flatten to Or([String("a"), String("b"), String("c"), String("d")])
    EXPECT_EQ(GRAMMATICA_TYPE_OR, simplified->type);
    const GrammaticaGrammarArray* simplified_subexprs = grammaticaGroupedGetSubexprs((GrammaticaGroupedGrammar*)simplified);
    EXPECT_EQ(4, simplified_subexprs->count);

    grammaticaGrammarUnref(simplified);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_a);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_b);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_c);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_d);
    grammaticaGrammarUnref((GrammaticaGrammar*)inner_or1);
    grammaticaGrammarUnref((GrammaticaGrammar*)inner_or2);
    grammaticaGrammarArrayDestroy(inner1);
    grammaticaGrammarArrayDestroy(inner2);
}

TEST_F(GrammarTest, ComplexSimplify) {
    // Grammar([Or([Or([Grammar([String("a"), String("b")]), Grammar([String("c"), String("d")])], (0,1)), 
    //              Or([Grammar([String("e"), String("f")]), Grammar([String("g"), String("h")])], (0,1))], (0,1))], (0,1))
    // Expected: Or([Or([String("ab"), String("cd")], (0,1)), Or([String("ef"), String("gh")], (0,1))])

    // Create inner grammars
    GrammaticaGrammarArray* ab_arr = grammaticaGrammarArrayCreate(0);
    grammaticaGrammarArrayAppend(ab_arr, (GrammaticaGrammar*)grammaticaStringCreate("a"));
    grammaticaGrammarArrayAppend(ab_arr, (GrammaticaGrammar*)grammaticaStringCreate("b"));
    GrammaticaGroupedGrammar* ab = grammaticaGrammarCreate(ab_arr, {1, 1});

    GrammaticaGrammarArray* cd_arr = grammaticaGrammarArrayCreate(0);
    grammaticaGrammarArrayAppend(cd_arr, (GrammaticaGrammar*)grammaticaStringCreate("c"));
    grammaticaGrammarArrayAppend(cd_arr, (GrammaticaGrammar*)grammaticaStringCreate("d"));
    GrammaticaGroupedGrammar* cd = grammaticaGrammarCreate(cd_arr, {1, 1});

    GrammaticaGrammarArray* or1_arr = grammaticaGrammarArrayCreate(0);
    grammaticaGrammarArrayAppend(or1_arr, (GrammaticaGrammar*)ab);
    grammaticaGrammarArrayAppend(or1_arr, (GrammaticaGrammar*)cd);
    GrammaticaGroupedGrammar* or1 = grammaticaOrCreate(or1_arr, {0, 1});

    GrammaticaGrammarArray* ef_arr = grammaticaGrammarArrayCreate(0);
    grammaticaGrammarArrayAppend(ef_arr, (GrammaticaGrammar*)grammaticaStringCreate("e"));
    grammaticaGrammarArrayAppend(ef_arr, (GrammaticaGrammar*)grammaticaStringCreate("f"));
    GrammaticaGroupedGrammar* ef = grammaticaGrammarCreate(ef_arr, {1, 1});

    GrammaticaGrammarArray* gh_arr = grammaticaGrammarArrayCreate(0);
    grammaticaGrammarArrayAppend(gh_arr, (GrammaticaGrammar*)grammaticaStringCreate("g"));
    grammaticaGrammarArrayAppend(gh_arr, (GrammaticaGrammar*)grammaticaStringCreate("h"));
    GrammaticaGroupedGrammar* gh = grammaticaGrammarCreate(gh_arr, {1, 1});

    GrammaticaGrammarArray* or2_arr = grammaticaGrammarArrayCreate(0);
    grammaticaGrammarArrayAppend(or2_arr, (GrammaticaGrammar*)ef);
    grammaticaGrammarArrayAppend(or2_arr, (GrammaticaGrammar*)gh);
    GrammaticaGroupedGrammar* or2 = grammaticaOrCreate(or2_arr, {0, 1});

    GrammaticaGrammarArray* or_outer_arr = grammaticaGrammarArrayCreate(0);
    grammaticaGrammarArrayAppend(or_outer_arr, (GrammaticaGrammar*)or1);
    grammaticaGrammarArrayAppend(or_outer_arr, (GrammaticaGrammar*)or2);
    GrammaticaGroupedGrammar* or_outer = grammaticaOrCreate(or_outer_arr, {0, 1});

    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)or_outer);
    grammar = grammaticaGrammarCreate(subexprs, {0, 1});

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    // Verify it's an Or
    EXPECT_EQ(GRAMMATICA_TYPE_OR, simplified->type);

    // Cleanup
    grammaticaGrammarUnref(simplified);
    grammaticaGrammarUnref((GrammaticaGrammar*)ab);
    grammaticaGrammarUnref((GrammaticaGrammar*)cd);
    grammaticaGrammarUnref((GrammaticaGrammar*)ef);
    grammaticaGrammarUnref((GrammaticaGrammar*)gh);
    grammaticaGrammarUnref((GrammaticaGrammar*)or1);
    grammaticaGrammarUnref((GrammaticaGrammar*)or2);
    grammaticaGrammarUnref((GrammaticaGrammar*)or_outer);
    grammaticaGrammarArrayDestroy(ab_arr);
    grammaticaGrammarArrayDestroy(cd_arr);
    grammaticaGrammarArrayDestroy(ef_arr);
    grammaticaGrammarArrayDestroy(gh_arr);
    grammaticaGrammarArrayDestroy(or1_arr);
    grammaticaGrammarArrayDestroy(or2_arr);
    grammaticaGrammarArrayDestroy(or_outer_arr);
}

TEST_F(GrammarTest, RenderEmpty) {
    grammar = grammaticaGrammarCreate(subexprs, {1, 1});
    
    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_EQ(nullptr, rendered);
}

TEST_F(GrammarTest, RenderGrammarWithOptionalQuantifier) {
    GrammaticaString* str1 = grammaticaStringCreate("a");
    GrammaticaString* str2 = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str2);

    grammar = grammaticaGrammarCreate(subexprs, {0, 1});

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("(\"a\" \"b\")?", rendered);
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
}

TEST_F(GrammarTest, RenderOrWithDifferentQuantifiers) {
    GrammaticaString* str1 = grammaticaStringCreate("a");
    GrammaticaString* str2 = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str2);

    grammar = grammaticaOrCreate(subexprs, {0, 0});  // *

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("(\"a\" | \"b\")*", rendered);
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
}

TEST_F(GrammarTest, NeedsWrapped_Grammar) {
    GrammaticaString* str = grammaticaStringCreate("a");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str);

    // Single subexpression - doesn't need wrap
    grammar = grammaticaGrammarCreate(subexprs, {1, 1});
    EXPECT_FALSE(grammaticaGroupedNeedsWrapped((GrammaticaGroupedGrammar*)grammar));
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Single subexpression with non-default quantifier - doesn't need wrap
    grammar = grammaticaGrammarCreate(subexprs, {0, 1});
    EXPECT_FALSE(grammaticaGroupedNeedsWrapped((GrammaticaGroupedGrammar*)grammar));
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Multiple subexpressions with default quantifier - doesn't need wrap
    GrammaticaString* str2 = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str2);
    grammar = grammaticaGrammarCreate(subexprs, {1, 1});
    EXPECT_FALSE(grammaticaGroupedNeedsWrapped((GrammaticaGroupedGrammar*)grammar));
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Multiple subexpressions with non-default quantifier - needs wrap
    grammar = grammaticaGrammarCreate(subexprs, {0, 1});
    EXPECT_TRUE(grammaticaGroupedNeedsWrapped((GrammaticaGroupedGrammar*)grammar));

    grammaticaGrammarUnref((GrammaticaGrammar*)str);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
}

TEST_F(GrammarTest, NeedsWrapped_Or) {
    GrammaticaString* str1 = grammaticaStringCreate("a");
    GrammaticaString* str2 = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str2);

    // Or always needs wrapped
    grammar = grammaticaOrCreate(subexprs, {1, 1});
    EXPECT_TRUE(grammaticaGroupedNeedsWrapped((GrammaticaGroupedGrammar*)grammar));

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
}

