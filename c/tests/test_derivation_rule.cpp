#include <gtest/gtest.h>

#include <string>

#include "grammatica.h"

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

TEST_F(DerivationRuleTest, SimpleRender) {
	String* value = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("value"));
	ASSERT_NE(value, nullptr);
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	ASSERT_NE(value_grammar, nullptr);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	char* rendered = grammatica_derivation_rule_render(ctx, rule, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "symbol ::= \"value\"");

	free(rendered);
	grammatica_derivation_rule_destroy(rule);
	grammatica_string_destroy(value);
}

TEST_F(DerivationRuleTest, EmptyValueReturnsNull) {
	String* value = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>(""));
	ASSERT_NE(value, nullptr);
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	EXPECT_EQ(value_grammar, nullptr);

	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", NULL);
	EXPECT_EQ(rule, nullptr);

	grammatica_string_destroy(value);
}

TEST_F(DerivationRuleTest, RenderNotFull) {
	String* value = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("value"));
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	char* rendered = grammatica_derivation_rule_render(ctx, rule, false, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "symbol");

	free(rendered);
	grammatica_derivation_rule_destroy(rule);
	grammatica_string_destroy(value);
}

TEST_F(DerivationRuleTest, GetMethods) {
	String* value = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("test_value"));
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "test_symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	const char* symbol = grammatica_derivation_rule_get_symbol(ctx, rule);
	ASSERT_NE(symbol, nullptr);
	EXPECT_STREQ(symbol, "test_symbol");

	const Grammar* got_value = grammatica_derivation_rule_get_value(ctx, rule);
	ASSERT_NE(got_value, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, got_value), GRAMMAR_TYPE_STRING);

	grammatica_derivation_rule_destroy(rule);
	grammatica_string_destroy(value);
}

TEST_F(DerivationRuleTest, AsString) {
	String* value = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("value"));
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	char* result = grammatica_derivation_rule_as_string(ctx, rule);
	ASSERT_NE(result, nullptr);
	EXPECT_NE(strstr(result, "symbol"), nullptr);
	EXPECT_NE(strstr(result, "DerivationRule"), nullptr);

	free(result);
	grammatica_derivation_rule_destroy(rule);
	grammatica_string_destroy(value);
}

TEST_F(DerivationRuleTest, Equals) {
	String* value1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("value"));
	String* value2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("value"));
	String* value3 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("different"));
	Grammar* grammar1 = grammatica_string_simplify(ctx, value1);
	Grammar* grammar2 = grammatica_string_simplify(ctx, value2);
	Grammar* grammar3 = grammatica_string_simplify(ctx, value3);
	DerivationRule* rule1 = grammatica_derivation_rule_create(ctx, "symbol", grammar1);
	DerivationRule* rule2 = grammatica_derivation_rule_create(ctx, "symbol", grammar2);
	DerivationRule* rule3 = grammatica_derivation_rule_create(ctx, "different", grammar3);
	ASSERT_NE(rule1, nullptr);
	ASSERT_NE(rule2, nullptr);
	ASSERT_NE(rule3, nullptr);

	EXPECT_TRUE(grammatica_derivation_rule_equals(ctx, rule1, rule2));
	EXPECT_FALSE(grammatica_derivation_rule_equals(ctx, rule1, rule3));
	EXPECT_TRUE(grammatica_derivation_rule_equals(ctx, rule1, rule1));

	grammatica_derivation_rule_destroy(rule1);
	grammatica_derivation_rule_destroy(rule2);
	grammatica_derivation_rule_destroy(rule3);
	grammatica_string_destroy(value1);
	grammatica_string_destroy(value2);
	grammatica_string_destroy(value3);
}

TEST_F(DerivationRuleTest, Copy) {
	String* value = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("value"));
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* original = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(original, nullptr);

	DerivationRule* copy = grammatica_derivation_rule_copy(ctx, original);
	ASSERT_NE(copy, nullptr);
	EXPECT_TRUE(grammatica_derivation_rule_equals(ctx, original, copy));
	EXPECT_NE(original, copy);

	grammatica_derivation_rule_destroy(original);
	grammatica_derivation_rule_destroy(copy);
	grammatica_string_destroy(value);
}

TEST_F(DerivationRuleTest, Simplify) {
	String* value = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("value"));
	Grammar* value_grammar = grammatica_string_simplify(ctx, value);
	DerivationRule* rule = grammatica_derivation_rule_create(ctx, "symbol", value_grammar);
	ASSERT_NE(rule, nullptr);

	Grammar* simplified = grammatica_derivation_rule_simplify(ctx, rule);
	ASSERT_NE(simplified, nullptr);
	EXPECT_EQ(grammatica_grammar_get_type(ctx, simplified), GRAMMAR_TYPE_DERIVATION_RULE);

	grammatica_grammar_destroy(simplified);
	grammatica_derivation_rule_destroy(rule);
	grammatica_string_destroy(value);
}
