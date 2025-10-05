#include <gtest/gtest.h>
#include <grammatica.h>

class DerivationRuleTest : public ::testing::Test {
protected:
    GrammaticaDerivationRule* rule = nullptr;
    GrammaticaString* value_grammar = nullptr;
    void SetUp() override {
        value_grammar = grammaticaStringCreate("test value");
        ASSERT_NE(nullptr, value_grammar);
    }
    void TearDown() override {
        if (rule != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)rule);
        }
        if (value_grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)value_grammar);
        }
    }
};

TEST_F(DerivationRuleTest, CreateAndGetters) {
    rule = grammaticaDerivationRuleCreate("my_rule", (GrammaticaGrammar*)value_grammar);
    ASSERT_NE(nullptr, rule);

    const char* symbol = grammaticaDerivationRuleGetSymbol(rule);
    EXPECT_STREQ("my_rule", symbol);

    const GrammaticaGrammar* value = grammaticaDerivationRuleGetValue(rule);
    EXPECT_EQ((GrammaticaGrammar*)value_grammar, value);
}

TEST_F(DerivationRuleTest, RenderFull) {
    rule = grammaticaDerivationRuleCreate("test", (GrammaticaGrammar*)value_grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)rule, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("test ::= \"test value\"", rendered);
    free(rendered);
}

TEST_F(DerivationRuleTest, RenderNotFull) {
    rule = grammaticaDerivationRuleCreate("test", (GrammaticaGrammar*)value_grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)rule, false, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("test", rendered);
    free(rendered);
}

TEST_F(DerivationRuleTest, Simplify) {
    rule = grammaticaDerivationRuleCreate("test", (GrammaticaGrammar*)value_grammar);

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)rule);
    ASSERT_NE(nullptr, simplified);

    EXPECT_EQ(GRAMMATICA_TYPE_DERIVATION_RULE, simplified->type);

    GrammaticaDerivationRule* simplified_rule = (GrammaticaDerivationRule*)simplified;
    EXPECT_STREQ("test", grammaticaDerivationRuleGetSymbol(simplified_rule));

    grammaticaGrammarUnref(simplified);
}

TEST_F(DerivationRuleTest, Copy) {
    rule = grammaticaDerivationRuleCreate("original", (GrammaticaGrammar*)value_grammar);

    GrammaticaGrammar* copied = grammaticaGrammarCopy((GrammaticaGrammar*)rule);
    ASSERT_NE(nullptr, copied);

    EXPECT_EQ(GRAMMATICA_TYPE_DERIVATION_RULE, copied->type);

    GrammaticaDerivationRule* copied_rule = (GrammaticaDerivationRule*)copied;
    EXPECT_STREQ("original", grammaticaDerivationRuleGetSymbol(copied_rule));

    grammaticaGrammarUnref(copied);
}

TEST_F(DerivationRuleTest, Equals) {
    rule = grammaticaDerivationRuleCreate("test", (GrammaticaGrammar*)value_grammar);

    GrammaticaDerivationRule* other = grammaticaDerivationRuleCreate("test", (GrammaticaGrammar*)value_grammar);
    GrammaticaString* different_value = grammaticaStringCreate("different");
    GrammaticaDerivationRule* different = grammaticaDerivationRuleCreate("test", (GrammaticaGrammar*)different_value);

    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)rule, (GrammaticaGrammar*)other, true));
    EXPECT_FALSE(grammaticaGrammarEquals((GrammaticaGrammar*)rule, (GrammaticaGrammar*)different, true));

    grammaticaGrammarUnref((GrammaticaGrammar*)other);
    grammaticaGrammarUnref((GrammaticaGrammar*)different);
    grammaticaGrammarUnref((GrammaticaGrammar*)different_value);
}

// Wrap behavior tests
TEST_F(DerivationRuleTest, RenderWrapOrMultipleSubexprs) {
    // Create Or with multiple subexpressions
    GrammaticaGrammarArray* subexprs = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str_a = grammaticaStringCreate("a");
    GrammaticaString* str_b = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str_a);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str_b);
    GrammaticaGroupedGrammar* or_value = grammaticaOrCreate(subexprs, {1, 1});

    rule = grammaticaDerivationRuleCreate("choice", (GrammaticaGrammar*)or_value);

    // With wrap=true, Or should be wrapped
    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)rule, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("choice ::= (\"a\" | \"b\")", rendered);
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str_a);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_b);
    grammaticaGrammarUnref((GrammaticaGrammar*)or_value);
    grammaticaGrammarArrayDestroy(subexprs);
}

TEST_F(DerivationRuleTest, RenderNoWrapOrMultipleSubexprs) {
    // Create Or with multiple subexpressions
    GrammaticaGrammarArray* subexprs = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str_a = grammaticaStringCreate("a");
    GrammaticaString* str_b = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str_a);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str_b);
    GrammaticaGroupedGrammar* or_value = grammaticaOrCreate(subexprs, {1, 1});

    rule = grammaticaDerivationRuleCreate("choice", (GrammaticaGrammar*)or_value);

    // With wrap=false, Or should not be wrapped
    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)rule, true, false);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("choice ::= \"a\" | \"b\"", rendered);
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str_a);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_b);
    grammaticaGrammarUnref((GrammaticaGrammar*)or_value);
    grammaticaGrammarArrayDestroy(subexprs);
}

TEST_F(DerivationRuleTest, RenderWrapOrSingleSubexpr) {
    // Create Or with single subexpression
    GrammaticaGrammarArray* subexprs = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str_a = grammaticaStringCreate("a");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str_a);
    GrammaticaGroupedGrammar* or_value = grammaticaOrCreate(subexprs, {1, 1});

    rule = grammaticaDerivationRuleCreate("choice", (GrammaticaGrammar*)or_value);

    // With wrap=true, single-item Or should not be wrapped (simplifies to just the item)
    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)rule, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("choice ::= \"a\"", rendered);
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str_a);
    grammaticaGrammarUnref((GrammaticaGrammar*)or_value);
    grammaticaGrammarArrayDestroy(subexprs);
}

TEST_F(DerivationRuleTest, RenderNoWrapGrammar) {
    // Create Grammar with multiple subexpressions
    GrammaticaGrammarArray* subexprs = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str_a = grammaticaStringCreate("a");
    GrammaticaString* str_b = grammaticaStringCreate("b");
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str_a);
    grammaticaGrammarArrayAppend(subexprs, (GrammaticaGrammar*)str_b);
    GrammaticaGroupedGrammar* grammar_value = grammaticaGrammarCreate(subexprs, {1, 1});

    rule = grammaticaDerivationRuleCreate("choice", (GrammaticaGrammar*)grammar_value);

    // Grammar should never be wrapped regardless of wrap parameter
    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)rule, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("choice ::= \"a\" \"b\"", rendered);
    free(rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)str_a);
    grammaticaGrammarUnref((GrammaticaGrammar*)str_b);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar_value);
    grammaticaGrammarArrayDestroy(subexprs);
}

TEST_F(DerivationRuleTest, RenderEmptyValue) {
    GrammaticaString* empty_value = grammaticaStringCreate("");
    rule = grammaticaDerivationRuleCreate("symbol", (GrammaticaGrammar*)empty_value);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)rule, true, true);
    EXPECT_EQ(nullptr, rendered);

    grammaticaGrammarUnref((GrammaticaGrammar*)empty_value);
}

TEST_F(DerivationRuleTest, SimplifyWithEmptyValue) {
    GrammaticaString* empty_value = grammaticaStringCreate("");
    rule = grammaticaDerivationRuleCreate("symbol", (GrammaticaGrammar*)empty_value);

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)rule);
    EXPECT_EQ(nullptr, simplified);

    grammaticaGrammarUnref((GrammaticaGrammar*)empty_value);
}

