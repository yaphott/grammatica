#include <gtest/gtest.h>
#include <grammatica.h>

class GrammarBaseTest : public ::testing::Test {
protected:
    GrammaticaString* str_grammar = nullptr;
    void SetUp() override {
        str_grammar = grammaticaStringCreate("test");
        ASSERT_NE(nullptr, str_grammar);
    }
    void TearDown() override {
        if (str_grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)str_grammar);
        }
    }
};

TEST_F(GrammarBaseTest, RefCounting) {
    GrammaticaGrammar* base = (GrammaticaGrammar*)str_grammar;

    // Initial ref count is 1
    grammaticaGrammarRef(base);
    // Now ref count is 2

    grammaticaGrammarUnref(base);
    // Back to 1

    // The TearDown will unref to 0 and free
}

TEST_F(GrammarBaseTest, ValidateQuantifier_Valid) {
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({1, 1}));
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({0, 1}));
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({0, 0}));  // 0 means infinity
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({1, 0}));  // 0 means infinity
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({2, 5}));
}

TEST_F(GrammarBaseTest, ValidateQuantifier_Invalid) {
    EXPECT_NE(GRAMMATICA_OK, grammaticaValidateQuantifier({5, 3}));  // min > max
}

TEST_F(GrammarBaseTest, RenderQuantifier) {
    char* result;

    result = grammaticaRenderQuantifier({1, 1});
    EXPECT_EQ(nullptr, result);  // Default doesn't render

    result = grammaticaRenderQuantifier({0, 1});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("?", result);
    free(result);

    result = grammaticaRenderQuantifier({0, 0});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("*", result);
    free(result);

    result = grammaticaRenderQuantifier({1, 0});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("+", result);
    free(result);

    result = grammaticaRenderQuantifier({2, 2});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("{2}", result);
    free(result);

    result = grammaticaRenderQuantifier({2, 5});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("{2,5}", result);
    free(result);

    result = grammaticaRenderQuantifier({2, 0});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("{2,}", result);
    free(result);
}

TEST(GrammarArrayTest, CreateAndDestroy) {
    GrammaticaGrammarArray* array = grammaticaGrammarArrayCreate(10);
    ASSERT_NE(nullptr, array);
    EXPECT_EQ(0, array->count);
    EXPECT_GE(array->capacity, 10);

    grammaticaGrammarArrayDestroy(array);
}

TEST(GrammarArrayTest, Append) {
    GrammaticaGrammarArray* array = grammaticaGrammarArrayCreate(2);
    ASSERT_NE(nullptr, array);

    GrammaticaString* str1 = grammaticaStringCreate("test1");
    GrammaticaString* str2 = grammaticaStringCreate("test2");
    GrammaticaString* str3 = grammaticaStringCreate("test3");

    ASSERT_EQ(GRAMMATICA_OK, grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str1));
    EXPECT_EQ(1, array->count);

    ASSERT_EQ(GRAMMATICA_OK, grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str2));
    EXPECT_EQ(2, array->count);

    // This should trigger a resize
    ASSERT_EQ(GRAMMATICA_OK, grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str3));
    EXPECT_EQ(3, array->count);

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
    grammaticaGrammarUnref((GrammaticaGrammar*)str3);

    grammaticaGrammarArrayDestroy(array);
}

TEST(GrammarArrayTest, Copy) {
    GrammaticaGrammarArray* array = grammaticaGrammarArrayCreate(0);

    GrammaticaString* str1 = grammaticaStringCreate("test1");
    GrammaticaString* str2 = grammaticaStringCreate("test2");

    grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str2);

    GrammaticaGrammarArray* copy = grammaticaGrammarArrayCopy(array);
    ASSERT_NE(nullptr, copy);
    EXPECT_EQ(array->count, copy->count);

    // Check that the items are the same
    for (size_t i = 0; i < array->count; i++) {
        EXPECT_EQ(array->items[i], copy->items[i]);
    }

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);

    grammaticaGrammarArrayDestroy(array);
    grammaticaGrammarArrayDestroy(copy);
}

// Additional quantifier validation tests
TEST_F(GrammarBaseTest, ValidateQuantifier_EdgeCases) {
    // Test additional valid cases
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({0, 2}));
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({0, 5}));
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({2, 0}));  // 0 means infinity
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({5, 5}));
    EXPECT_EQ(GRAMMATICA_OK, grammaticaValidateQuantifier({2, 3}));
}

TEST_F(GrammarBaseTest, RenderQuantifier_AllCases) {
    char* result;

    // Test {0,2} quantifier
    result = grammaticaRenderQuantifier({0, 2});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("{0,2}", result);
    free(result);

    // Test {1,3} quantifier
    result = grammaticaRenderQuantifier({1, 3});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("{1,3}", result);
    free(result);

    // Test {0,} (0 or more with no limit)
    result = grammaticaRenderQuantifier({0, 0});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("*", result);
    free(result);

    // Test {1,} (1 or more with no limit)
    result = grammaticaRenderQuantifier({1, 0});
    ASSERT_NE(nullptr, result);
    EXPECT_STREQ("+", result);
    free(result);
}

TEST_F(GrammarBaseTest, Equals_SameType) {
    GrammaticaString* str1 = grammaticaStringCreate("test");
    GrammaticaString* str2 = grammaticaStringCreate("test");
    GrammaticaString* str3 = grammaticaStringCreate("different");

    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)str1, (GrammaticaGrammar*)str2, true));
    EXPECT_FALSE(grammaticaGrammarEquals((GrammaticaGrammar*)str1, (GrammaticaGrammar*)str3, true));

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
    grammaticaGrammarUnref((GrammaticaGrammar*)str3);
}

TEST_F(GrammarBaseTest, Equals_DifferentTypes) {
    GrammaticaString* str = grammaticaStringCreate("test");
    GrammaticaCharRangeEntry range = {'a', 'z'};
    GrammaticaCharRange* char_range = grammaticaCharRangeCreate(&range, 1, false);

    EXPECT_FALSE(grammaticaGrammarEquals((GrammaticaGrammar*)str, (GrammaticaGrammar*)char_range, true));

    grammaticaGrammarUnref((GrammaticaGrammar*)str);
    grammaticaGrammarUnref((GrammaticaGrammar*)char_range);
}

TEST_F(GrammarBaseTest, Equals_WithQuantifier) {
    GrammaticaGrammarArray* subexprs1 = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str1 = grammaticaStringCreate("a");
    grammaticaGrammarArrayAppend(subexprs1, (GrammaticaGrammar*)str1);
    GrammaticaGroupedGrammar* grammar1 = grammaticaGrammarCreate(subexprs1, {1, 1});

    GrammaticaGrammarArray* subexprs2 = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str2 = grammaticaStringCreate("a");
    grammaticaGrammarArrayAppend(subexprs2, (GrammaticaGrammar*)str2);
    GrammaticaGroupedGrammar* grammar2 = grammaticaGrammarCreate(subexprs2, {1, 1});

    GrammaticaGrammarArray* subexprs3 = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str3 = grammaticaStringCreate("a");
    grammaticaGrammarArrayAppend(subexprs3, (GrammaticaGrammar*)str3);
    GrammaticaGroupedGrammar* grammar3 = grammaticaGrammarCreate(subexprs3, {2, 2});

    // Should be equal with same quantifier
    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar1, (GrammaticaGrammar*)grammar2, true));

    // Should not be equal with different quantifier when checking
    EXPECT_FALSE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar1, (GrammaticaGrammar*)grammar3, true));

    // Should be equal with different quantifier when not checking
    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar1, (GrammaticaGrammar*)grammar3, false));

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
    grammaticaGrammarUnref((GrammaticaGrammar*)str3);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar1);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar2);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar3);
    grammaticaGrammarArrayDestroy(subexprs1);
    grammaticaGrammarArrayDestroy(subexprs2);
    grammaticaGrammarArrayDestroy(subexprs3);
}

TEST_F(GrammarBaseTest, Equals_SameInstance) {
    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)str_grammar, (GrammaticaGrammar*)str_grammar, true));
}

TEST_F(GrammarBaseTest, Equals_NullPointer) {
    EXPECT_FALSE(grammaticaGrammarEquals((GrammaticaGrammar*)str_grammar, nullptr, true));
    EXPECT_FALSE(grammaticaGrammarEquals(nullptr, (GrammaticaGrammar*)str_grammar, true));
    EXPECT_FALSE(grammaticaGrammarEquals(nullptr, nullptr, true));
}

TEST_F(GrammarBaseTest, Copy) {
    GrammaticaGrammar* copied = grammaticaGrammarCopy((GrammaticaGrammar*)str_grammar);
    ASSERT_NE(nullptr, copied);
    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)str_grammar, copied, true));

    grammaticaGrammarUnref(copied);
}

