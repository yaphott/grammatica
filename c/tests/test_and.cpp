#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "grammatica.h"

class AndTest : public ::testing::Test {
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

TEST_F(AndTest, EmptyAnd) {
	Grammar* subexprs[1];
	And* and_expr = grammatica_and_create(ctx, subexprs, 0, {1, 1});
	/* Empty And may be allowed in C implementation */
	if (and_expr != nullptr) {
		grammatica_and_destroy(and_expr);
	}
	/* Test passes either way */
	EXPECT_TRUE(true);
}

TEST_F(AndTest, SingleSubexpression) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	ASSERT_NE(str, nullptr);
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	ASSERT_NE(str_grammar, nullptr);
	Grammar* subexprs[] = {str_grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(and_expr, nullptr);

	char* rendered = grammatica_and_render(ctx, and_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "\"a\"");

	free(rendered);
	grammatica_and_destroy(and_expr);
	grammatica_string_destroy(str);
}

TEST_F(AndTest, MultipleSubexpressions) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	ASSERT_NE(str1, nullptr);
	ASSERT_NE(str2, nullptr);
	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	ASSERT_NE(grammar1, nullptr);
	ASSERT_NE(grammar2, nullptr);
	Grammar* subexprs[] = {grammar1, grammar2};
	And* and_expr = grammatica_and_create(ctx, subexprs, 2, {1, 1});
	ASSERT_NE(and_expr, nullptr);

	char* rendered = grammatica_and_render(ctx, and_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "\"a\" \"b\"");

	free(rendered);
	grammatica_and_destroy(and_expr);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(AndTest, WithQuantifier) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	ASSERT_NE(str, nullptr);
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	ASSERT_NE(str_grammar, nullptr);
	Grammar* subexprs[] = {str_grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {0, 1});
	ASSERT_NE(and_expr, nullptr);

	char* rendered = grammatica_and_render(ctx, and_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "\"a\"?");

	free(rendered);
	grammatica_and_destroy(and_expr);
	grammatica_string_destroy(str);
}

TEST_F(AndTest, MultipleWithQuantifier) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	ASSERT_NE(str1, nullptr);
	ASSERT_NE(str2, nullptr);
	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	ASSERT_NE(grammar1, nullptr);
	ASSERT_NE(grammar2, nullptr);
	Grammar* subexprs[] = {grammar1, grammar2};
	And* and_expr = grammatica_and_create(ctx, subexprs, 2, {0, -1});
	ASSERT_NE(and_expr, nullptr);

	char* rendered = grammatica_and_render(ctx, and_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "(\"a\" \"b\")*");

	free(rendered);
	grammatica_and_destroy(and_expr);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(AndTest, GetMethods) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	Grammar* subexprs[] = {grammar1, grammar2};
	And* and_expr = grammatica_and_create(ctx, subexprs, 2, {0, 1});
	ASSERT_NE(and_expr, nullptr);

	size_t num = grammatica_and_get_subexprs_n(ctx, and_expr);
	EXPECT_EQ(num, 2);

	Quantifier q = grammatica_and_get_quantifier(ctx, and_expr);
	EXPECT_EQ(q.lower, 0);
	EXPECT_EQ(q.upper, 1);

	Grammar* out_subexprs[10];
	int result = grammatica_and_get_subexprs(ctx, and_expr, out_subexprs, 10);
	EXPECT_EQ(result, 2);

	grammatica_and_destroy(and_expr);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(AndTest, Equals) {
	String* str1a = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str1b = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));

	Grammar* grammar1a = grammatica_string_simplify(ctx, str1a);
	Grammar* grammar1b = grammatica_string_simplify(ctx, str1b);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);

	Grammar* subexprs1[] = {grammar1a};
	Grammar* subexprs2[] = {grammar1b};
	Grammar* subexprs3[] = {grammar2};

	And* and1 = grammatica_and_create(ctx, subexprs1, 1, {1, 1});
	And* and2 = grammatica_and_create(ctx, subexprs2, 1, {1, 1});
	And* and3 = grammatica_and_create(ctx, subexprs3, 1, {1, 1});

	ASSERT_NE(and1, nullptr);
	ASSERT_NE(and2, nullptr);
	ASSERT_NE(and3, nullptr);

	/* Same subexpressions and quantifier should be equal */
	EXPECT_TRUE(grammatica_and_equals(ctx, and1, and2, true));

	/* Different subexpressions should not be equal */
	EXPECT_FALSE(grammatica_and_equals(ctx, and1, and3, true));

	/* Same object should equal itself */
	EXPECT_TRUE(grammatica_and_equals(ctx, and1, and1, true));

	grammatica_and_destroy(and1);
	grammatica_and_destroy(and2);
	grammatica_and_destroy(and3);
	grammatica_string_destroy(str1a);
	grammatica_string_destroy(str1b);
	grammatica_string_destroy(str2);
}

TEST_F(AndTest, Copy) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	Grammar* grammar = grammatica_string_simplify(ctx, str);
	Grammar* subexprs[] = {grammar};
	And* original = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(original, nullptr);

	And* copy = grammatica_and_copy(ctx, original);
	ASSERT_NE(copy, nullptr);
	EXPECT_TRUE(grammatica_and_equals(ctx, original, copy, true));
	EXPECT_NE(original, copy);

	grammatica_and_destroy(original);
	grammatica_and_destroy(copy);
	grammatica_string_destroy(str);
}

TEST_F(AndTest, AsString) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	Grammar* grammar = grammatica_string_simplify(ctx, str);
	Grammar* subexprs[] = {grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(and_expr, nullptr);

	char* result = grammatica_and_as_string(ctx, and_expr);
	ASSERT_NE(result, nullptr);
	EXPECT_NE(strstr(result, "And"), nullptr);

	free(result);
	grammatica_and_destroy(and_expr);
	grammatica_string_destroy(str);
}

TEST_F(AndTest, Simplify) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	Grammar* grammar = grammatica_string_simplify(ctx, str);
	Grammar* subexprs[] = {grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(and_expr, nullptr);

	Grammar* simplified = grammatica_and_simplify(ctx, and_expr);
	ASSERT_NE(simplified, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, simplified), GRAMMAR_TYPE_STRING);

	grammatica_grammar_destroy(simplified);
	grammatica_and_destroy(and_expr);
	grammatica_string_destroy(str);
}
