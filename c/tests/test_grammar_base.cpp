#include <gtest/gtest.h>

#include <string>

#include "grammatica.h"

/* Test fixture for GrammarBase tests */
class GrammarBaseTest : public ::testing::Test {
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

/* Test: Context init and finish */
TEST_F(GrammarBaseTest, ContextInitAndFinish) {
	GrammaticaContextHandle_t new_ctx = grammatica_init();
	ASSERT_NE(new_ctx, nullptr);

	/* Should be able to create grammars with new context */
	String* str = grammatica_string_create(new_ctx, "test");
	EXPECT_NE(str, nullptr);

	grammatica_string_destroy(new_ctx, str);
	grammatica_finish(new_ctx);
}

/* Test: Grammar get_type polymorphism */
TEST_F(GrammarBaseTest, GetType) {
	/* String */
	String* str = grammatica_string_create(ctx, "a");
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	ASSERT_NE(str_grammar, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, str_grammar), GRAMMAR_TYPE_STRING);
	grammatica_grammar_destroy(ctx, str_grammar);
	grammatica_string_destroy(ctx, str);

	/* CharRange */
	const char* chars = "az";
	CharRange* cr = grammatica_char_range_from_chars(ctx, chars, strlen(chars), false);
	Grammar* cr_grammar = grammatica_char_range_simplify(ctx, cr);
	ASSERT_NE(cr_grammar, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, cr_grammar), GRAMMAR_TYPE_CHAR_RANGE);
	grammatica_grammar_destroy(ctx, cr_grammar);
	grammatica_char_range_destroy(ctx, cr);

	/* DerivationRule */
	String* str2 = grammatica_string_create(ctx, "value");
	Grammar* value_grammar = grammatica_string_simplify(ctx, str2);
	DerivationRule* dr = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	Grammar* dr_grammar = grammatica_derivation_rule_simplify(ctx, dr);
	ASSERT_NE(dr_grammar, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, dr_grammar), GRAMMAR_TYPE_DERIVATION_RULE);
	grammatica_grammar_destroy(ctx, dr_grammar);
	grammatica_derivation_rule_destroy(ctx, dr);
	grammatica_string_destroy(ctx, str2);

	/* And */
	String* str3 = grammatica_string_create(ctx, "b");
	Grammar* str3_grammar = grammatica_string_simplify(ctx, str3);
	Grammar* subexprs[] = {str3_grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	Grammar* and_grammar = grammatica_and_simplify(ctx, and_expr);
	ASSERT_NE(and_grammar, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, and_grammar), GRAMMAR_TYPE_STRING); /* Simplifies to String */
	grammatica_grammar_destroy(ctx, and_grammar);
	grammatica_and_destroy(ctx, and_expr);
	grammatica_string_destroy(ctx, str3);

	/* Or */
	String* str4 = grammatica_string_create(ctx, "c");
	Grammar* str4_grammar = grammatica_string_simplify(ctx, str4);
	Grammar* or_subexprs[] = {str4_grammar};
	Or* or_expr = grammatica_or_create(ctx, or_subexprs, 1, {1, 1});
	Grammar* or_grammar = grammatica_or_simplify(ctx, or_expr);
	ASSERT_NE(or_grammar, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, or_grammar), GRAMMAR_TYPE_STRING); /* Simplifies to String */
	grammatica_grammar_destroy(ctx, or_grammar);
	grammatica_or_destroy(ctx, or_expr);
	grammatica_string_destroy(ctx, str4);
}

/* Test: Grammar render polymorphism */
TEST_F(GrammarBaseTest, RenderPolymorphism) {
	/* String */
	String* str = grammatica_string_create(ctx, "test");
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	char* str_rendered = grammatica_grammar_render(ctx, str_grammar, true, true);
	ASSERT_NE(str_rendered, nullptr);
	EXPECT_STREQ(str_rendered, "\"test\"");

	/* CharRange */
	const char* chars = "az";
	CharRange* cr = grammatica_char_range_from_chars(ctx, chars, strlen(chars), false);
	Grammar* cr_grammar = grammatica_char_range_simplify(ctx, cr);
	char* cr_rendered = grammatica_grammar_render(ctx, cr_grammar, true, true);
	ASSERT_NE(cr_rendered, nullptr);
	EXPECT_STREQ(cr_rendered, "[az]");

	grammatica_free_string(ctx, str_rendered);
	grammatica_free_string(ctx, cr_rendered);
	grammatica_grammar_destroy(ctx, str_grammar);
	grammatica_grammar_destroy(ctx, cr_grammar);
	grammatica_string_destroy(ctx, str);
	grammatica_char_range_destroy(ctx, cr);
}

/* Test: Grammar simplify polymorphism */
TEST_F(GrammarBaseTest, SimplifyPolymorphism) {
	/* String */
	String* str = grammatica_string_create(ctx, "a");
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	Grammar* str_simplified = grammatica_grammar_simplify(ctx, str_grammar);
	ASSERT_NE(str_simplified, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, str_simplified), GRAMMAR_TYPE_STRING);

	/* And with single subexpression simplifies to that subexpression */
	String* str2 = grammatica_string_create(ctx, "b");
	Grammar* str2_grammar = grammatica_string_simplify(ctx, str2);
	Grammar* subexprs[] = {str2_grammar};
	And* and_expr = grammatica_and_create(ctx, subexprs, 1, {1, 1});
	Grammar* and_grammar = grammatica_and_simplify(ctx, and_expr);
	Grammar* and_simplified = grammatica_grammar_simplify(ctx, and_grammar);
	ASSERT_NE(and_simplified, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, and_simplified), GRAMMAR_TYPE_STRING);

	grammatica_grammar_destroy(ctx, str_simplified);
	grammatica_grammar_destroy(ctx, and_simplified);
	grammatica_grammar_destroy(ctx, str_grammar);
	grammatica_grammar_destroy(ctx, and_grammar);
	grammatica_string_destroy(ctx, str);
	grammatica_string_destroy(ctx, str2);
	grammatica_and_destroy(ctx, and_expr);
}

/* Test: Grammar as_string polymorphism */
TEST_F(GrammarBaseTest, AsStringPolymorphism) {
	/* String */
	String* str = grammatica_string_create(ctx, "test");
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	char* str_as_string = grammatica_grammar_as_string(ctx, str_grammar);
	ASSERT_NE(str_as_string, nullptr);
	EXPECT_NE(strstr(str_as_string, "String"), nullptr);
	EXPECT_NE(strstr(str_as_string, "test"), nullptr);

	/* CharRange */
	const char* chars = "az";
	CharRange* cr = grammatica_char_range_from_chars(ctx, chars, strlen(chars), false);
	Grammar* cr_grammar = grammatica_char_range_simplify(ctx, cr);
	char* cr_as_string = grammatica_grammar_as_string(ctx, cr_grammar);
	ASSERT_NE(cr_as_string, nullptr);
	EXPECT_NE(strstr(cr_as_string, "CharRange"), nullptr);

	grammatica_free_string(ctx, str_as_string);
	grammatica_free_string(ctx, cr_as_string);
	grammatica_grammar_destroy(ctx, str_grammar);
	grammatica_grammar_destroy(ctx, cr_grammar);
	grammatica_string_destroy(ctx, str);
	grammatica_char_range_destroy(ctx, cr);
}

/* Test: Grammar equals polymorphism */
TEST_F(GrammarBaseTest, EqualsPolymorphism) {
	/* Same String grammars */
	String* str1 = grammatica_string_create(ctx, "test");
	String* str2 = grammatica_string_create(ctx, "test");
	Grammar* grammar1 = grammatica_string_simplify(ctx, str1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, str2);

	EXPECT_TRUE(grammatica_grammar_equals(ctx, grammar1, grammar2));
	EXPECT_TRUE(grammatica_grammar_equals(ctx, grammar1, grammar1)); /* Same instance */

	/* Different String grammars */
	String* str3 = grammatica_string_create(ctx, "different");
	Grammar* grammar3 = grammatica_string_simplify(ctx, str3);
	EXPECT_FALSE(grammatica_grammar_equals(ctx, grammar1, grammar3));

	/* Different types */
	const char* chars = "az";
	CharRange* cr = grammatica_char_range_from_chars(ctx, chars, strlen(chars), false);
	Grammar* cr_grammar = grammatica_char_range_simplify(ctx, cr);
	EXPECT_FALSE(grammatica_grammar_equals(ctx, grammar1, cr_grammar));

	/* Null handling */
	EXPECT_FALSE(grammatica_grammar_equals(ctx, grammar1, nullptr));
	EXPECT_FALSE(grammatica_grammar_equals(ctx, nullptr, grammar1));

	grammatica_grammar_destroy(ctx, grammar1);
	grammatica_grammar_destroy(ctx, grammar2);
	grammatica_grammar_destroy(ctx, grammar3);
	grammatica_grammar_destroy(ctx, cr_grammar);
	grammatica_string_destroy(ctx, str1);
	grammatica_string_destroy(ctx, str2);
	grammatica_string_destroy(ctx, str3);
	grammatica_char_range_destroy(ctx, cr);
}

/* Test: Grammar copy polymorphism */
TEST_F(GrammarBaseTest, CopyPolymorphism) {
	/* String */
	String* str = grammatica_string_create(ctx, "test");
	Grammar* original = grammatica_string_simplify(ctx, str);
	Grammar* copy = grammatica_grammar_copy(ctx, original);

	ASSERT_NE(copy, nullptr);
	EXPECT_NE(original, copy);                                   /* Different instances */
	EXPECT_TRUE(grammatica_grammar_equals(ctx, original, copy)); /* But equal */
	EXPECT_EQ(grammatica_grammar_get_type(ctx, copy), GRAMMAR_TYPE_STRING);

	/* CharRange */
	const char* chars = "az";
	CharRange* cr = grammatica_char_range_from_chars(ctx, chars, strlen(chars), false);
	Grammar* cr_original = grammatica_char_range_simplify(ctx, cr);
	Grammar* cr_copy = grammatica_grammar_copy(ctx, cr_original);

	ASSERT_NE(cr_copy, nullptr);
	EXPECT_NE(cr_original, cr_copy);
	EXPECT_TRUE(grammatica_grammar_equals(ctx, cr_original, cr_copy));
	EXPECT_EQ(grammatica_grammar_get_type(ctx, cr_copy), GRAMMAR_TYPE_CHAR_RANGE);

	grammatica_grammar_destroy(ctx, copy);
	grammatica_grammar_destroy(ctx, original);
	grammatica_grammar_destroy(ctx, cr_copy);
	grammatica_grammar_destroy(ctx, cr_original);
	grammatica_string_destroy(ctx, str);
	grammatica_char_range_destroy(ctx, cr);
}

/* Test: Grammar destroy polymorphism */
TEST_F(GrammarBaseTest, DestroyPolymorphism) {
	/* Create various grammar types and destroy them through the base interface */
	String* str = grammatica_string_create(ctx, "test");
	Grammar* str_grammar = grammatica_string_simplify(ctx, str);
	grammatica_grammar_destroy(ctx, str_grammar);

	const char* chars = "az";
	CharRange* cr = grammatica_char_range_from_chars(ctx, chars, strlen(chars), false);
	Grammar* cr_grammar = grammatica_char_range_simplify(ctx, cr);
	grammatica_grammar_destroy(ctx, cr_grammar);

	String* str2 = grammatica_string_create(ctx, "value");
	Grammar* value_grammar = grammatica_string_simplify(ctx, str2);
	DerivationRule* dr = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	Grammar* dr_grammar = grammatica_derivation_rule_simplify(ctx, dr);
	grammatica_grammar_destroy(ctx, dr_grammar);

	/* Cleanup base objects */
	grammatica_string_destroy(ctx, str);
	grammatica_char_range_destroy(ctx, cr);
	grammatica_derivation_rule_destroy(ctx, dr);
	grammatica_string_destroy(ctx, str2);

	/* Test passes if no crashes/leaks */
	EXPECT_TRUE(true);
}

/* Test: Null context handling */
TEST_F(GrammarBaseTest, NullContextHandling) {
	/* All functions should handle null context gracefully */
	EXPECT_EQ(grammatica_grammar_get_type(nullptr, nullptr), (GrammarType)-1);
	EXPECT_EQ(grammatica_grammar_render(nullptr, nullptr, true, true), nullptr);
	EXPECT_EQ(grammatica_grammar_simplify(nullptr, nullptr), nullptr);
	EXPECT_EQ(grammatica_grammar_as_string(nullptr, nullptr), nullptr);
	EXPECT_FALSE(grammatica_grammar_equals(nullptr, nullptr, nullptr));
	EXPECT_EQ(grammatica_grammar_copy(nullptr, nullptr), nullptr);

	/* Destroy with null context should not crash */
	grammatica_grammar_destroy(nullptr, nullptr);
	grammatica_free_string(nullptr, nullptr);
	grammatica_finish(nullptr);

	EXPECT_TRUE(true);
}
