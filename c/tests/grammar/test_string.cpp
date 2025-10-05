#include <gtest/gtest.h>
#include <grammatica.h>

class StringGrammarTest : public ::testing::Test {
protected:
    GrammaticaString* grammar = nullptr;
    void TearDown() override {
        if (grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
        }
    }
};

TEST_F(StringGrammarTest, CreateAndGetValue) {
    grammar = grammaticaStringCreate("Hello");
    ASSERT_NE(nullptr, grammar);

    const char* value = grammaticaStringGetValue(grammar);
    EXPECT_STREQ("Hello", value);
}

TEST_F(StringGrammarTest, RenderSimple) {
    grammar = grammaticaStringCreate("abc");

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("\"abc\"", rendered);
    free(rendered);
}

TEST_F(StringGrammarTest, RenderEmpty) {
    grammar = grammaticaStringCreate("");

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_EQ(nullptr, rendered);
}

TEST_F(StringGrammarTest, RenderWithEscapes) {
    grammar = grammaticaStringCreate("Hello\nWorld");

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("\"Hello\\nWorld\"", rendered);
    free(rendered);
}

TEST_F(StringGrammarTest, RenderWithQuotes) {
    grammar = grammaticaStringCreate("Say \"Hello\"");

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("\"Say \\\"Hello\\\"\"", rendered);
    free(rendered);
}

TEST_F(StringGrammarTest, Simplify) {
    grammar = grammaticaStringCreate("test");

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    EXPECT_EQ(GRAMMATICA_TYPE_STRING, simplified->type);
    GrammaticaString* simplified_str = (GrammaticaString*)simplified;
    EXPECT_STREQ("test", grammaticaStringGetValue(simplified_str));

    grammaticaGrammarUnref(simplified);
}

TEST_F(StringGrammarTest, SimplifyEmpty) {
    grammar = grammaticaStringCreate("");

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    EXPECT_EQ(nullptr, simplified);
}

TEST_F(StringGrammarTest, Copy) {
    grammar = grammaticaStringCreate("original");

    GrammaticaGrammar* copied = grammaticaGrammarCopy((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, copied);

    EXPECT_EQ(GRAMMATICA_TYPE_STRING, copied->type);
    GrammaticaString* copied_str = (GrammaticaString*)copied;
    EXPECT_STREQ("original", grammaticaStringGetValue(copied_str));

    grammaticaGrammarUnref(copied);
}

TEST_F(StringGrammarTest, Equals) {
    grammar = grammaticaStringCreate("test");
    GrammaticaString* other = grammaticaStringCreate("test");
    GrammaticaString* different = grammaticaStringCreate("other");

    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar, (GrammaticaGrammar*)other, true));
    EXPECT_FALSE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar, (GrammaticaGrammar*)different, true));

    grammaticaGrammarUnref((GrammaticaGrammar*)other);
    grammaticaGrammarUnref((GrammaticaGrammar*)different);
}

TEST(MergeAdjacentStringsTest, NoStrings) {
    GrammaticaGrammarArray* array = grammaticaGrammarArrayCreate(0);

    EXPECT_EQ(GRAMMATICA_OK, grammaticaMergeAdjacentStrings(array));
    EXPECT_EQ(0, array->count);

    grammaticaGrammarArrayDestroy(array);
}

TEST(MergeAdjacentStringsTest, SingleString) {
    GrammaticaGrammarArray* array = grammaticaGrammarArrayCreate(0);
    GrammaticaString* str = grammaticaStringCreate("test");

    grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str);

    EXPECT_EQ(GRAMMATICA_OK, grammaticaMergeAdjacentStrings(array));
    EXPECT_EQ(1, array->count);
    EXPECT_STREQ("test", grammaticaStringGetValue((GrammaticaString*)array->items[0]));

    grammaticaGrammarUnref((GrammaticaGrammar*)str);
    grammaticaGrammarArrayDestroy(array);
}

TEST(MergeAdjacentStringsTest, MultipleAdjacentStrings) {
    GrammaticaGrammarArray* array = grammaticaGrammarArrayCreate(0);

    GrammaticaString* str1 = grammaticaStringCreate("Hello");
    GrammaticaString* str2 = grammaticaStringCreate(" ");
    GrammaticaString* str3 = grammaticaStringCreate("World");

    grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str1);
    grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str2);
    grammaticaGrammarArrayAppend(array, (GrammaticaGrammar*)str3);

    EXPECT_EQ(GRAMMATICA_OK, grammaticaMergeAdjacentStrings(array));
    EXPECT_EQ(1, array->count);
    EXPECT_STREQ("Hello World", grammaticaStringGetValue((GrammaticaString*)array->items[0]));

    grammaticaGrammarUnref((GrammaticaGrammar*)str1);
    grammaticaGrammarUnref((GrammaticaGrammar*)str2);
    grammaticaGrammarUnref((GrammaticaGrammar*)str3);
    grammaticaGrammarArrayDestroy(array);
}
