#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "grammatica.h"
#include "grammatica_constants.h"

class StringTest : public ::testing::Test {
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

std::string get_all_always_safe_chars() {
	std::string result;
	const char* digits = "0123456789";
	const char* ascii_letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char* punctuation = "!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
	const char* space = " ";
	result += digits;
	result += ascii_letters;
	result += punctuation;
	result += space;
	return result;
}

std::string get_all_char_escape_map_chars() {
	return "\n\r\t";
}

std::string get_all_string_literal_escape_chars() {
	return "\"\\";
}

struct StringRenderTestCase {
	std::string description;
	std::string input;
	std::string expected; /* Empty string means expected to return NULL */
	bool expect_null;
};

class StringRenderTest : public StringTest, public ::testing::WithParamInterface<StringRenderTestCase> {};

TEST_P(StringRenderTest, Render) {
	auto test_case = GetParam();

	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>(test_case.input.c_str()));
	ASSERT_NE(str, nullptr);

	unsigned char* result = grammatica_string_render(ctx, str);

	if (test_case.expect_null) {
		EXPECT_EQ(result, nullptr) << "Description: " << test_case.description;
	} else {
		ASSERT_NE(result, nullptr) << "Description: " << test_case.description;
		EXPECT_STREQ(reinterpret_cast<const char*>(result), test_case.expected.c_str()) << "Description: " << test_case.description;
		free(reinterpret_cast<char*>(result));
	}

	grammatica_string_destroy(str);
}

INSTANTIATE_TEST_SUITE_P(
    StringRenderTests,
    StringRenderTest,
    ::testing::Values(StringRenderTestCase{"Empty", "", "", true},
                      StringRenderTestCase{"Single character", "a", "\"a\"", false},
                      StringRenderTestCase{"Multiple characters", "abc", "\"abc\"", false},
                      StringRenderTestCase{
                          "Always safe characters", get_all_always_safe_chars(), "\"" + get_all_always_safe_chars() + "\"", false},
                      StringRenderTestCase{"Escape general tokens", "\n\r\t", "\"\\n\\r\\t\"", false},
                      StringRenderTestCase{"Escape string literal characters", "\"\\", "\"\\\"\\\\\"", false},
                      StringRenderTestCase{"Escape non-ASCII characters (0x7F)", "\x7F", "\"\\x7F\"", false}));

struct StringSimplifyTestCase {
	std::string description;
	std::string input;
	std::string expected;
	bool expect_null;
};

class StringSimplifyTest : public StringTest, public ::testing::WithParamInterface<StringSimplifyTestCase> {};

TEST_P(StringSimplifyTest, Simplify) {
	auto test_case = GetParam();

	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>(test_case.input.c_str()));
	ASSERT_NE(str, nullptr);

	Grammar* result = grammatica_string_simplify(ctx, str);

	if (test_case.expect_null) {
		EXPECT_EQ(result, nullptr) << "Description: " << test_case.description;
	} else {
		ASSERT_NE(result, nullptr) << "Description: " << test_case.description;
		EXPECT_EQ(grammatica_grammar_get_type(ctx, result), GRAMMAR_TYPE_STRING) << "Description: " << test_case.description;

		/* Verify by rendering the result */
		char* rendered = grammatica_grammar_render(ctx, result, true, true);
		ASSERT_NE(rendered, nullptr);

		std::string expected_render = "\"" + test_case.expected + "\"";
		EXPECT_STREQ(rendered, expected_render.c_str()) << "Description: " << test_case.description;

		free(rendered);
		grammatica_grammar_destroy(result);
	}

	grammatica_string_destroy(str);
}

INSTANTIATE_TEST_SUITE_P(StringSimplifyTests,
                         StringSimplifyTest,
                         ::testing::Values(StringSimplifyTestCase{"Empty", "", "", true},
                                           StringSimplifyTestCase{"Single character", "a", "a", false},
                                           StringSimplifyTestCase{"Multiple characters", "abc", "abc", false}));

TEST_F(StringTest, AsString) {
	String* str = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("test"));
	ASSERT_NE(str, nullptr);

	unsigned char* result = grammatica_string_as_string(ctx, str);
	ASSERT_NE(result, nullptr);

	EXPECT_STREQ(reinterpret_cast<const char*>(result), "String(value='test')");

	free(reinterpret_cast<char*>(result));
	grammatica_string_destroy(str);
}

TEST_F(StringTest, Equals) {
	String* str1 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("test"));
	String* str2 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("test"));
	String* str3 = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("different"));

	ASSERT_NE(str1, nullptr);
	ASSERT_NE(str2, nullptr);
	ASSERT_NE(str3, nullptr);

	/* Same value should be equal */
	EXPECT_TRUE(grammatica_string_equals(ctx, str1, str2));

	/* Different values should not be equal */
	EXPECT_FALSE(grammatica_string_equals(ctx, str1, str3));

	/* Same object should be equal to itself */
	EXPECT_TRUE(grammatica_string_equals(ctx, str1, str1));

	grammatica_string_destroy(str1);
	grammatica_string_destroy(str2);
	grammatica_string_destroy(str3);
}

TEST_F(StringTest, Copy) {
	String* original = grammatica_string_create(ctx, reinterpret_cast<const unsigned char*>("test"));
	ASSERT_NE(original, nullptr);

	String* copy = grammatica_string_copy(ctx, original);
	ASSERT_NE(copy, nullptr);

	/* Copy should have same value */
	EXPECT_TRUE(grammatica_string_equals(ctx, original, copy));

	/* But should be different objects */
	EXPECT_NE(original, copy);

	grammatica_string_destroy(original);
	grammatica_string_destroy(copy);
}

TEST_F(StringTest, NullHandling) {
	/* Creating with NULL value should fail */
	String* str = grammatica_string_create(ctx, NULL);
	EXPECT_EQ(str, nullptr);

	/* Destroying NULL should not crash */
	grammatica_string_destroy(NULL);

	/* Rendering NULL should return NULL */
	unsigned char* result = grammatica_string_render(ctx, NULL);
	EXPECT_EQ(result, nullptr);
}
