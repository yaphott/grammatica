#include <gtest/gtest.h>

#include <string>

#include "grammatica.h"
#include "grammatica_group.h"

class GroupBaseTest : public ::testing::Test {
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

struct QuantifierRenderTestCase {
	Quantifier quantifier;
	const char* expected;
};

class QuantifierRenderTest : public GroupBaseTest, public ::testing::WithParamInterface<QuantifierRenderTestCase> {};

TEST_P(QuantifierRenderTest, Render) {
	QuantifierRenderTestCase tc = GetParam();

	char* rendered = render_quantifier(tc.quantifier);

	if (tc.expected == nullptr) {
		EXPECT_EQ(rendered, nullptr);
	} else {
		ASSERT_NE(rendered, nullptr);
		EXPECT_STREQ(rendered, tc.expected);
		free(rendered);
	}
}

INSTANTIATE_TEST_SUITE_P(QuantifierRenderTests,
                         QuantifierRenderTest,
                         ::testing::Values(QuantifierRenderTestCase{{1, 1}, nullptr},  // Default quantifier - no render
                                           QuantifierRenderTestCase{{0, 1}, "?"},      // Optional
                                           QuantifierRenderTestCase{{0, 2}, "{0,2}"},  // Custom range
                                           QuantifierRenderTestCase{{0, -1}, "*"},     // Zero or more
                                           QuantifierRenderTestCase{{1, -1}, "+"},     // One or more
                                           QuantifierRenderTestCase{{2, 2}, "{2}"},    // Exactly 2
                                           QuantifierRenderTestCase{{2, 5}, "{2,5}"},  // Range 2-5
                                           QuantifierRenderTestCase{{2, -1}, "{2,}"}   // Two or more
                                           ));

TEST_F(GroupBaseTest, QuantifierNeedsWrap) {
	/* Default quantifier (1, 1) does not need wrapping */
	EXPECT_FALSE(quantifier_needs_wrap({1, 1}));

	/* Any other quantifier needs wrapping */
	EXPECT_TRUE(quantifier_needs_wrap({0, 1}));
	EXPECT_TRUE(quantifier_needs_wrap({0, -1}));
	EXPECT_TRUE(quantifier_needs_wrap({1, -1}));
	EXPECT_TRUE(quantifier_needs_wrap({2, 2}));
	EXPECT_TRUE(quantifier_needs_wrap({2, 5}));
	EXPECT_TRUE(quantifier_needs_wrap({2, -1}));
}

TEST_F(GroupBaseTest, AndQuantifierNormalization) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str3 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	ASSERT_NE(str1, nullptr);
	ASSERT_NE(str2, nullptr);
	ASSERT_NE(str3, nullptr);

	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	Grammar* grammar3 = grammatica_string_simplify(ctx, str3);
	ASSERT_NE(grammar1, nullptr);
	ASSERT_NE(grammar2, nullptr);
	ASSERT_NE(grammar3, nullptr);

	/* Test integer quantifier - should normalize to (n, n) */
	Grammar* subexprs1[] = {grammar1};
	And* and1 = grammatica_and_create(ctx, subexprs1, 1, {5, 5});
	ASSERT_NE(and1, nullptr);

	Quantifier q1 = grammatica_and_get_quantifier(ctx, and1);
	EXPECT_EQ(q1.lower, 5);
	EXPECT_EQ(q1.upper, 5);

	/* Test tuple quantifier */
	Grammar* subexprs2[] = {grammar2};
	And* and2 = grammatica_and_create(ctx, subexprs2, 1, {0, 1});
	ASSERT_NE(and2, nullptr);

	Quantifier q2 = grammatica_and_get_quantifier(ctx, and2);
	EXPECT_EQ(q2.lower, 0);
	EXPECT_EQ(q2.upper, 1);

	/* Test infinity quantifier */
	Grammar* subexprs3[] = {grammar3};
	And* and3 = grammatica_and_create(ctx, subexprs3, 1, {1, -1});
	ASSERT_NE(and3, nullptr);

	Quantifier q3 = grammatica_and_get_quantifier(ctx, and3);
	EXPECT_EQ(q3.lower, 1);
	EXPECT_EQ(q3.upper, -1);

	grammatica_and_destroy(and1);
	grammatica_and_destroy(and2);
	grammatica_and_destroy(and3);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
	grammatica_string_destroy(str3);
}

TEST_F(GroupBaseTest, OrQuantifierNormalization) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	ASSERT_NE(str1, nullptr);
	ASSERT_NE(str2, nullptr);

	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	ASSERT_NE(grammar1, nullptr);
	ASSERT_NE(grammar2, nullptr);

	/* Test various quantifier forms */
	Grammar* subexprs1[] = {grammar1};
	Or* or1 = grammatica_or_create(ctx, subexprs1, 1, {2, 5});
	ASSERT_NE(or1, nullptr);

	Quantifier q1 = grammatica_or_get_quantifier(ctx, or1);
	EXPECT_EQ(q1.lower, 2);
	EXPECT_EQ(q1.upper, 5);

	Grammar* subexprs2[] = {grammar2};
	Or* or2 = grammatica_or_create(ctx, subexprs2, 1, {2, -1});
	ASSERT_NE(or2, nullptr);

	Quantifier q2 = grammatica_or_get_quantifier(ctx, or2);
	EXPECT_EQ(q2.lower, 2);
	EXPECT_EQ(q2.upper, -1);

	grammatica_or_destroy(or1);
	grammatica_or_destroy(or2);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(GroupBaseTest, AndRenderingWithWrapping) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));
	String* str3 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str4 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));

	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	Grammar* grammar3 = grammatica_string_simplify(ctx, str3);
	Grammar* grammar4 = grammatica_string_simplify(ctx, str4);

	/* Multiple subexpressions with default quantifier - no parentheses by default */
	Grammar* subexprs1[] = {grammar1, grammar2};
	And* and1 = grammatica_and_create(ctx, subexprs1, 2, {1, 1});
	ASSERT_NE(and1, nullptr);

	char* rendered1 = grammatica_and_render(ctx, and1, true, true);
	ASSERT_NE(rendered1, nullptr);
	EXPECT_STREQ(rendered1, "\"a\" \"b\"");

	/* With quantifier, wrapping is used */
	Grammar* subexprs2[] = {grammar3, grammar4};
	And* and2 = grammatica_and_create(ctx, subexprs2, 2, {0, 1});
	ASSERT_NE(and2, nullptr);

	char* rendered2 = grammatica_and_render(ctx, and2, true, true);
	ASSERT_NE(rendered2, nullptr);
	EXPECT_STREQ(rendered2, "(\"a\" \"b\")?");

	free(rendered1);
	free(rendered2);
	grammatica_and_destroy(and1);
	grammatica_and_destroy(and2);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
	grammatica_string_destroy(str3);
	grammatica_string_destroy(str4);
}

TEST_F(GroupBaseTest, OrRenderingWithWrapping) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("b"));

	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);

	/* Multiple subexpressions with default quantifier - uses parentheses */
	Grammar* subexprs1[] = {grammar1, grammar2};
	Or* or1 = grammatica_or_create(ctx, subexprs1, 2, {1, 1});
	ASSERT_NE(or1, nullptr);

	char* rendered1 = grammatica_or_render(ctx, or1, true, true);
	ASSERT_NE(rendered1, nullptr);
	EXPECT_STREQ(rendered1, "(\"a\" | \"b\")");

	/* Without wrapping flag */
	char* rendered2 = grammatica_or_render(ctx, or1, true, false);
	ASSERT_NE(rendered2, nullptr);
	EXPECT_STREQ(rendered2, "\"a\" | \"b\"");

	free(rendered1);
	free(rendered2);
	grammatica_or_destroy(or1);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
}

TEST_F(GroupBaseTest, AndQuantifierRendering) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str3 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str4 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));

	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	Grammar* grammar3 = grammatica_string_simplify(ctx, str3);
	Grammar* grammar4 = grammatica_string_simplify(ctx, str4);

	/* Test optional */
	Grammar* subexprs1[] = {grammar1};
	And* and1 = grammatica_and_create(ctx, subexprs1, 1, {0, 1});
	char* rendered1 = grammatica_and_render(ctx, and1, true, true);
	ASSERT_NE(rendered1, nullptr);
	EXPECT_STREQ(rendered1, "\"a\"?");

	/* Test star */
	Grammar* subexprs2[] = {grammar2};
	And* and2 = grammatica_and_create(ctx, subexprs2, 1, {0, -1});
	char* rendered2 = grammatica_and_render(ctx, and2, true, true);
	ASSERT_NE(rendered2, nullptr);
	EXPECT_STREQ(rendered2, "\"a\"*");

	/* Test plus */
	Grammar* subexprs3[] = {grammar3};
	And* and3 = grammatica_and_create(ctx, subexprs3, 1, {1, -1});
	char* rendered3 = grammatica_and_render(ctx, and3, true, true);
	ASSERT_NE(rendered3, nullptr);
	EXPECT_STREQ(rendered3, "\"a\"+");

	/* Test custom range */
	Grammar* subexprs4[] = {grammar4};
	And* and4 = grammatica_and_create(ctx, subexprs4, 1, {2, 5});
	char* rendered4 = grammatica_and_render(ctx, and4, true, true);
	ASSERT_NE(rendered4, nullptr);
	EXPECT_STREQ(rendered4, "\"a\"{2,5}");

	free(rendered1);
	free(rendered2);
	free(rendered3);
	free(rendered4);
	grammatica_and_destroy(and1);
	grammatica_and_destroy(and2);
	grammatica_and_destroy(and3);
	grammatica_and_destroy(and4);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
	grammatica_string_destroy(str3);
	grammatica_string_destroy(str4);
}

TEST_F(GroupBaseTest, OrQuantifierRendering) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str3 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));
	String* str4 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("a"));

	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);
	Grammar* grammar3 = grammatica_string_simplify(ctx, str3);
	Grammar* grammar4 = grammatica_string_simplify(ctx, str4);

	/* Test optional */
	Grammar* subexprs1[] = {grammar1};
	Or* or1 = grammatica_or_create(ctx, subexprs1, 1, {0, 1});
	char* rendered1 = grammatica_or_render(ctx, or1, true, true);
	ASSERT_NE(rendered1, nullptr);
	EXPECT_STREQ(rendered1, "\"a\"?");

	/* Test star */
	Grammar* subexprs2[] = {grammar2};
	Or* or2 = grammatica_or_create(ctx, subexprs2, 1, {0, -1});
	char* rendered2 = grammatica_or_render(ctx, or2, true, true);
	ASSERT_NE(rendered2, nullptr);
	EXPECT_STREQ(rendered2, "\"a\"*");

	/* Test plus */
	Grammar* subexprs3[] = {grammar3};
	Or* or3 = grammatica_or_create(ctx, subexprs3, 1, {1, -1});
	char* rendered3 = grammatica_or_render(ctx, or3, true, true);
	ASSERT_NE(rendered3, nullptr);
	EXPECT_STREQ(rendered3, "\"a\"+");

	/* Test exact count */
	Grammar* subexprs4[] = {grammar4};
	Or* or4 = grammatica_or_create(ctx, subexprs4, 1, {2, 2});
	char* rendered4 = grammatica_or_render(ctx, or4, true, true);
	ASSERT_NE(rendered4, nullptr);
	EXPECT_STREQ(rendered4, "\"a\"{2}");

	free(rendered1);
	free(rendered2);
	free(rendered3);
	free(rendered4);
	grammatica_or_destroy(or1);
	grammatica_or_destroy(or2);
	grammatica_or_destroy(or3);
	grammatica_or_destroy(or4);
	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
	grammatica_string_destroy(str3);
	grammatica_string_destroy(str4);
}
