#include <gtest/gtest.h>

#include <string>

#include "grammatica.h"

/* Test fixture for DerivationRule tests */
class DerivationRuleTest : public ::testing::Test {
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

/* Test: Simple derivation rule */
TEST_F(DerivationRuleTest, SimpleRender) {
	String* value = grammatica_string_create(ctx, "value");
	ASSERT_NE(value, nullptr);

	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	ASSERT_NE(value_grammar, nullptr);

	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	char* rendered = grammatica_derivation_rule_render(ctx, rule, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "symbol ::= \"value\"");

	grammatica_free_string(ctx, rendered);
	grammatica_derivation_rule_destroy(ctx, rule);
	grammatica_string_destroy(ctx, value);
}

/* Test: Empty value returns None */
TEST_F(DerivationRuleTest, EmptyValueReturnsNull) {
	String* value = grammatica_string_create(ctx, "");
	ASSERT_NE(value, nullptr);

	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	/* Empty string simplifies to NULL */
	EXPECT_EQ(value_grammar, nullptr);

	/* Cannot create derivation rule with NULL value */
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", NULL);
	EXPECT_EQ(rule, nullptr);

	grammatica_string_destroy(ctx, value);
}

/* Test: Render with full=false returns just the symbol */
TEST_F(DerivationRuleTest, RenderNotFull) {
	String* value = grammatica_string_create(ctx, "value");
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	char* rendered = grammatica_derivation_rule_render(ctx, rule, false, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "symbol");

	grammatica_free_string(ctx, rendered);
	grammatica_derivation_rule_destroy(ctx, rule);
	grammatica_string_destroy(ctx, value);
}

/* Test: get_symbol and get_value */
TEST_F(DerivationRuleTest, GetMethods) {
	String* value = grammatica_string_create(ctx, "test_value");
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "test_symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	const char* symbol = grammatica_derivation_rule_get_symbol(ctx, rule);
	ASSERT_NE(symbol, nullptr);
	EXPECT_STREQ(symbol, "test_symbol");

	const Grammar* got_value = grammatica_derivation_rule_get_value(ctx, rule);
	ASSERT_NE(got_value, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, got_value), GRAMMAR_TYPE_STRING);

	grammatica_derivation_rule_destroy(ctx, rule);
	grammatica_string_destroy(ctx, value);
}

/* Test: as_string */
TEST_F(DerivationRuleTest, AsString) {
	String* value = grammatica_string_create(ctx, "value");
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	char* result = grammatica_derivation_rule_as_string(ctx, rule);
	ASSERT_NE(result, nullptr);

	/* Should contain both symbol and value information */
	EXPECT_NE(strstr(result, "symbol"), nullptr);
	EXPECT_NE(strstr(result, "DerivationRule"), nullptr);

	grammatica_free_string(ctx, result);
	grammatica_derivation_rule_destroy(ctx, rule);
	grammatica_string_destroy(ctx, value);
}

/* Test: equals */
TEST_F(DerivationRuleTest, Equals) {
	String* value1 = grammatica_string_create(ctx, "value");
	String* value2 = grammatica_string_create(ctx, "value");
	String* value3 = grammatica_string_create(ctx, "different");

	Grammar* grammar1 = grammatica_string_simplify(ctx, value1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, value2);
	Grammar* grammar3 = grammatica_string_simplify(ctx, value3);

	DerivationRule* rule1 = grammatica_derivation_rule_create(ctx, "symbol", grammar1);
	DerivationRule* rule2 = grammatica_derivation_rule_create(ctx, "symbol", grammar2);
	DerivationRule* rule3 = grammatica_derivation_rule_create(ctx, "different", grammar3);

	ASSERT_NE(rule1, nullptr);
	ASSERT_NE(rule2, nullptr);
	ASSERT_NE(rule3, nullptr);

	/* Same symbol and value should be equal */
	EXPECT_TRUE(grammatica_derivation_rule_equals(ctx, rule1, rule2));

	/* Different symbol or value should not be equal */
	EXPECT_FALSE(grammatica_derivation_rule_equals(ctx, rule1, rule3));

	/* Same object should equal itself */
	EXPECT_TRUE(grammatica_derivation_rule_equals(ctx, rule1, rule1));

	grammatica_derivation_rule_destroy(ctx, rule1);
	grammatica_derivation_rule_destroy(ctx, rule2);
	grammatica_derivation_rule_destroy(ctx, rule3);
	grammatica_string_destroy(ctx, value1);
	grammatica_string_destroy(ctx, value2);
	grammatica_string_destroy(ctx, value3);
}

/* Test: copy */
TEST_F(DerivationRuleTest, Copy) {
	String* value = grammatica_string_create(ctx, "value");
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* original = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(original, nullptr);

	DerivationRule* copy = grammatica_derivation_rule_copy(ctx, original);
	ASSERT_NE(copy, nullptr);

	/* Copy should be equal */
	EXPECT_TRUE(grammatica_derivation_rule_equals(ctx, original, copy));

	/* But should be different objects */
	EXPECT_NE(original, copy);

	grammatica_derivation_rule_destroy(ctx, original);
	grammatica_derivation_rule_destroy(ctx, copy);
	grammatica_string_destroy(ctx, value);
}

/* Test: simplify */
TEST_F(DerivationRuleTest, Simplify) {
	String* value = grammatica_string_create(ctx, "value");
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	Grammar* simplified = grammatica_derivation_rule_simplify(ctx, rule);
	ASSERT_NE(simplified, nullptr);

	/* Simplified should be a derivation rule */
	EXPECT_EQ(grammatica_grammar_get_type(ctx, simplified), GRAMMAR_TYPE_DERIVATION_RULE);

	grammatica_grammar_destroy(ctx, simplified);
	grammatica_derivation_rule_destroy(ctx, rule);
	grammatica_string_destroy(ctx, value);
}
