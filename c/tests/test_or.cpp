#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "grammatica.h"

class OrTest : public ::testing::Test {
   protected:
	GrammaticaContextHandle_t ctx;

	void SetUp() override {
		ctx = grammatica_init();
		ASSERT_NE(ctx, nullptr);
	}

	void TearDown() override {
		grammatica_finish(ctx);
	}
};

TEST_F(OrTest, SingleSubexpression) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	ASSERT_NE(str, nullptr);
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	ASSERT_NE(str_grammar, nullptr);
	Grammar* subexprs[] = {str_grammar};
	Or* or_expr = grammatica_or_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(or_expr, nullptr);

	char* rendered = grammatica_or_render(ctx, or_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "\"a\"");

	free(rendered);
	grammatica_or_destroy(or_expr);
	grammatica_string_destroy(str);
}

TEST_F(OrTest, MultipleSubexpressions) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	ASSERT_NE(str1, nullptr);
	ASSERT_NE(str2, nullptr);
	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	ASSERT_NE(grammar1, nullptr);
	ASSERT_NE(grammar2, nullptr);
	Grammar* subexprs[] = {grammar1, grammar2};
	Or* or_expr = grammatica_or_create(ctx, subexprs, 2, {1, 1});
	ASSERT_NE(or_expr, nullptr);

	char* rendered = grammatica_or_render(ctx, or_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "(\"a\" | \"b\")");

	free(rendered);
	grammatica_or_destroy(or_expr);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(OrTest, WithQuantifier) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	ASSERT_NE(str, nullptr);
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	ASSERT_NE(str_grammar, nullptr);
	Grammar* subexprs[] = {str_grammar};
	Or* or_expr = grammatica_or_create(ctx, subexprs, 1, {0, 1});
	ASSERT_NE(or_expr, nullptr);

	char* rendered = grammatica_or_render(ctx, or_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "\"a\"?");

	free(rendered);
	grammatica_or_destroy(or_expr);
	grammatica_string_destroy(str);
}

TEST_F(OrTest, MultipleWithQuantifier) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	ASSERT_NE(str1, nullptr);
	ASSERT_NE(str2, nullptr);
	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	ASSERT_NE(grammar1, nullptr);
	ASSERT_NE(grammar2, nullptr);
	Grammar* subexprs[] = {grammar1, grammar2};
	Or* or_expr = grammatica_or_create(ctx, subexprs, 2, {0, -1});
	ASSERT_NE(or_expr, nullptr);

	char* rendered = grammatica_or_render(ctx, or_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "(\"a\" | \"b\")*");

	free(rendered);
	grammatica_or_destroy(or_expr);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(OrTest, WithoutWrapping) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	Grammar* subexprs[] = {grammar1, grammar2};
	Or* or_expr = grammatica_or_create(ctx, subexprs, 2, {1, 1});
	ASSERT_NE(or_expr, nullptr);

	char* rendered = grammatica_or_render(ctx, or_expr, true, false);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "\"a\" | \"b\"");

	free(rendered);
	grammatica_or_destroy(or_expr);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(OrTest, GetMethods) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	Grammar* subexprs[] = {grammar1, grammar2};
	Or* or_expr = grammatica_or_create(ctx, subexprs, 2, {0, 1});
	ASSERT_NE(or_expr, nullptr);

	size_t num = grammatica_or_get_subexprs_n(ctx, or_expr);
	EXPECT_EQ(num, 2);

	Quantifier q = grammatica_or_get_quantifier(ctx, or_expr);
	EXPECT_EQ(q.lower, 0);
	EXPECT_EQ(q.upper, 1);

	Grammar* out_subexprs[10];
	int result = grammatica_or_get_subexprs(ctx, or_expr, out_subexprs, 10);
	EXPECT_EQ(result, 2);

	grammatica_or_destroy(or_expr);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(OrTest, Equals) {
	String* str1a = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str1b = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	Grammar* grammar1a = grammatica_string_simplify(ctx, str1a);
	Grammar* grammar1b = grammatica_string_simplify(ctx, str1b);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	Grammar* subexprs1[] = {grammar1a};
	Grammar* subexprs2[] = {grammar1b};
	Grammar* subexprs3[] = {grammar2};
	Or* or1 = grammatica_or_create(ctx, subexprs1, 1, {1, 1});
	Or* or2 = grammatica_or_create(ctx, subexprs2, 1, {1, 1});
	Or* or3 = grammatica_or_create(ctx, subexprs3, 1, {1, 1});
	ASSERT_NE(or1, nullptr);
	ASSERT_NE(or2, nullptr);
	ASSERT_NE(or3, nullptr);

	/* Same subexpressions and quantifier should be equal */
	EXPECT_TRUE(grammatica_or_equals(ctx, or1, or2, true));
	EXPECT_FALSE(grammatica_or_equals(ctx, or1, or3, true));
	EXPECT_TRUE(grammatica_or_equals(ctx, or1, or1, true));

	grammatica_or_destroy(or1);
	grammatica_or_destroy(or2);
	grammatica_or_destroy(or3);
	grammatica_string_destroy(str1a);
	grammatica_string_destroy(str1b);
	grammatica_string_destroy(str2);
}

TEST_F(OrTest, Copy) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	Grammar* grammar = grammatica_string_simplify(ctx, str);
	Grammar* subexprs[] = {grammar};
	Or* original = grammatica_or_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(original, nullptr);

	Or* copy = grammatica_or_copy(ctx, original);
	ASSERT_NE(copy, nullptr);
	EXPECT_TRUE(grammatica_or_equals(ctx, original, copy, true));
	EXPECT_NE(original, copy);

	grammatica_or_destroy(original);
	grammatica_or_destroy(copy);
	grammatica_string_destroy(str);
}

TEST_F(OrTest, AsString) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	Grammar* grammar = grammatica_string_simplify(ctx, str);
	Grammar* subexprs[] = {grammar};
	Or* or_expr = grammatica_or_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(or_expr, nullptr);

	char* result = grammatica_or_as_string(ctx, or_expr);
	ASSERT_NE(result, nullptr);
	EXPECT_NE(strstr(result, "Or"), nullptr);

	free(result);
	grammatica_or_destroy(or_expr);
	grammatica_string_destroy(str);
}

TEST_F(OrTest, Simplify) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	Grammar* grammar = grammatica_string_simplify(ctx, str);
	Grammar* subexprs[] = {grammar};
	Or* or_expr = grammatica_or_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(or_expr, nullptr);

	Grammar* simplified = grammatica_or_simplify(ctx, or_expr);
	ASSERT_NE(simplified, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, simplified), GRAMMAR_TYPE_STRING);

	grammatica_grammar_destroy(simplified);
	grammatica_or_destroy(or_expr);
	grammatica_string_destroy(str);
}
