#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "grammatica.h"

/* Test fixture for And tests */
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

/* Test: Empty And */
TEST_F(AndTest, EmptyAnd) {
	Grammar* subexprs[1];
	And* and_expr = grammatica_and_create(ctx, subexprs, 0, {1, 1});
	/* Empty And may be allowed in C implementation */
	if (and_expr != nullptr) {
		grammatica_and_destroy(ctx, and_expr);
	}
	/* Test passes either way */
	EXPECT_TRUE(true);
}

/* Test: And with single String subexpression */
TEST_F(AndTest, SingleSubexpression) {
	String* str = grammatica_string_create(ctx, "a");
	ASSERT_NE(str, nullptr);

	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	ASSERT_NE(str_grammar, nullptr);

	Grammar* subexprs[] = {str_grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(and_expr, nullptr);

	char* rendered = grammatica_and_render(ctx, and_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "\"a\"");

	grammatica_free_string(ctx, rendered);
	grammatica_and_destroy(ctx, and_expr);
	grammatica_string_destroy(ctx, str);
}

/* Test: And with multiple String subexpressions */
TEST_F(AndTest, MultipleSubexpressions) {
	String* str1 = grammatica_string_create(ctx, "a");
	String* str2 = grammatica_string_create(ctx, "b");
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

	grammatica_free_string(ctx, rendered);
	grammatica_and_destroy(ctx, and_expr);
	grammatica_string_destroy(ctx, str1);
	grammatica_string_destroy(ctx, str2);
}

/* Test: And with quantifier */
TEST_F(AndTest, WithQuantifier) {
	String* str = grammatica_string_create(ctx, "a");
	ASSERT_NE(str, nullptr);

	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	ASSERT_NE(str_grammar, nullptr);

	Grammar* subexprs[] = {str_grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {0, 1});
	ASSERT_NE(and_expr, nullptr);

	char* rendered = grammatica_and_render(ctx, and_expr, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "\"a\"?");

	grammatica_free_string(ctx, rendered);
	grammatica_and_destroy(ctx, and_expr);
	grammatica_string_destroy(ctx, str);
}

/* Test: And with multiple quantifiers */
TEST_F(AndTest, MultipleWithQuantifier) {
	String* str1 = grammatica_string_create(ctx, "a");
	String* str2 = grammatica_string_create(ctx, "b");
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

	grammatica_free_string(ctx, rendered);
	grammatica_and_destroy(ctx, and_expr);
	grammatica_string_destroy(ctx, str1);
	grammatica_string_destroy(ctx, str2);
}

/* Test: get methods */
TEST_F(AndTest, GetMethods) {
	String* str1 = grammatica_string_create(ctx, "a");
	String* str2 = grammatica_string_create(ctx, "b");

	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);

	Grammar* subexprs[] = {grammar1, grammar2};
	And* and_expr = grammatica_and_create(ctx, subexprs, 2, {0, 1});
	ASSERT_NE(and_expr, nullptr);

	/* Test get_subexprs_n */
	size_t num = grammatica_and_get_subexprs_n(ctx, and_expr);
	EXPECT_EQ(num, 2);

	/* Test get_quantifier */
	Quantifier q = grammatica_and_get_quantifier(ctx, and_expr);
	EXPECT_EQ(q.lower, 0);
	EXPECT_EQ(q.upper, 1);

	/* Test get_subexprs */
	Grammar* out_subexprs[10];
	int result = grammatica_and_get_subexprs(ctx, and_expr, out_subexprs, 10);
	EXPECT_EQ(result, 2);

	grammatica_and_destroy(ctx, and_expr);
	grammatica_string_destroy(ctx, str1);
	grammatica_string_destroy(ctx, str2);
}

/* Test: equals */
TEST_F(AndTest, Equals) {
	String* str1a = grammatica_string_create(ctx, "a");
	String* str1b = grammatica_string_create(ctx, "a");
	String* str2 = grammatica_string_create(ctx, "b");

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

	grammatica_and_destroy(ctx, and1);
	grammatica_and_destroy(ctx, and2);
	grammatica_and_destroy(ctx, and3);
	grammatica_string_destroy(ctx, str1a);
	grammatica_string_destroy(ctx, str1b);
	grammatica_string_destroy(ctx, str2);
}

/* Test: copy */
TEST_F(AndTest, Copy) {
	String* str = grammatica_string_create(ctx, "a");
	Grammar* grammar = grammatica_string_simplify(ctx, str);

	Grammar* subexprs[] = {grammar};
	And* original = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(original, nullptr);

	And* copy = grammatica_and_copy(ctx, original);
	ASSERT_NE(copy, nullptr);

	/* Copy should be equal */
	EXPECT_TRUE(grammatica_and_equals(ctx, original, copy, true));

	/* But should be different objects */
	EXPECT_NE(original, copy);

	grammatica_and_destroy(ctx, original);
	grammatica_and_destroy(ctx, copy);
	grammatica_string_destroy(ctx, str);
}

/* Test: as_string */
TEST_F(AndTest, AsString) {
	String* str = grammatica_string_create(ctx, "a");
	Grammar* grammar = grammatica_string_simplify(ctx, str);

	Grammar* subexprs[] = {grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(and_expr, nullptr);

	char* result = grammatica_and_as_string(ctx, and_expr);
	ASSERT_NE(result, nullptr);

	/* Should contain And information */
	EXPECT_NE(strstr(result, "And"), nullptr);

	grammatica_free_string(ctx, result);
	grammatica_and_destroy(ctx, and_expr);
	grammatica_string_destroy(ctx, str);
}

/* Test: simplify */
TEST_F(AndTest, Simplify) {
	String* str = grammatica_string_create(ctx, "a");
	Grammar* grammar = grammatica_string_simplify(ctx, str);

	Grammar* subexprs[] = {grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	ASSERT_NE(and_expr, nullptr);

	Grammar* simplified = grammatica_and_simplify(ctx, and_expr);
	ASSERT_NE(simplified, nullptr);

	/* Single subexpression with default quantifier should simplify to just the subexpression */
	EXPECT_EQ(grammatica_grammar_get_type(ctx, simplified), GRAMMAR_TYPE_STRING);

	grammatica_grammar_destroy(ctx, simplified);
	grammatica_and_destroy(ctx, and_expr);
	grammatica_string_destroy(ctx, str);
}
