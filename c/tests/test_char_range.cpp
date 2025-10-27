#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "grammatica.h"
#include "grammatica_constants.h"

/* Test fixture for CharRange tests */
class CharRangeTest : public ::testing::Test {
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

/* Test: Empty char_ranges should fail */
TEST_F(CharRangeTest, ValidationEmpty) {
	CharRange* range = grammatica_char_range_create(ctx, NULL, 0, false);
	EXPECT_EQ(range, nullptr);
}

/* Test: from_chars method */
TEST_F(CharRangeTest, FromChars) {
	const char* chars = "abcxyz";
	CharRange* range = grammatica_char_range_from_chars(ctx, chars, strlen(chars), false);
	ASSERT_NE(range, nullptr);

	/* Expected: [a-c] and [x-z] which should merge to [a-cx-z] */
	char* rendered = grammatica_char_range_render(ctx, range, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "[a-cx-z]");

	grammatica_free_string(ctx, rendered);
	grammatica_char_range_destroy(ctx, range);
}

/* Test: from_chars negated */
TEST_F(CharRangeTest, FromCharsNegated) {
	const char* chars = "xyz";
	CharRange* range = grammatica_char_range_from_chars(ctx, chars, strlen(chars), true);
	ASSERT_NE(range, nullptr);

	char* rendered = grammatica_char_range_render(ctx, range, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "[^x-z]");

	grammatica_free_string(ctx, rendered);
	grammatica_char_range_destroy(ctx, range);
}

/* Test: from_ords method */
TEST_F(CharRangeTest, FromOrds) {
	int ords[] = {97, 98, 99, 120, 121, 122}; /* a,b,c,x,y,z */
	CharRange* range = grammatica_char_range_from_ords(ctx, ords, 6, false);
	ASSERT_NE(range, nullptr);

	char* rendered = grammatica_char_range_render(ctx, range, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "[a-cx-z]");

	grammatica_free_string(ctx, rendered);
	grammatica_char_range_destroy(ctx, range);
}

/* Test: from_ords negated */
TEST_F(CharRangeTest, FromOrdsNegated) {
	int ords[] = {97, 98, 99, 120, 121, 122};
	CharRange* range = grammatica_char_range_from_ords(ctx, ords, 6, true);
	ASSERT_NE(range, nullptr);

	char* rendered = grammatica_char_range_render(ctx, range, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, "[^a-cx-z]");

	grammatica_free_string(ctx, rendered);
	grammatica_char_range_destroy(ctx, range);
}

/* Test cases for CharRange simplify */
struct CharRangeSimplifyTestCase {
	std::string description;
	std::vector<CharRangePair> ranges;
	bool negate;
	GrammarType expected_type; /* GRAMMAR_TYPE_STRING or GRAMMAR_TYPE_CHAR_RANGE */
	std::string expected_render;
};

class CharRangeSimplifyTest : public CharRangeTest, public ::testing::WithParamInterface<CharRangeSimplifyTestCase> {};

TEST_P(CharRangeSimplifyTest, Simplify) {
	auto test_case = GetParam();

	CharRange* range = grammatica_char_range_create(ctx, test_case.ranges.data(), test_case.ranges.size(), test_case.negate);
	ASSERT_NE(range, nullptr);

	Grammar* result = grammatica_char_range_simplify(ctx, range);
	ASSERT_NE(result, nullptr);

	EXPECT_EQ(grammatica_grammar_get_type(ctx, result), test_case.expected_type) << "Description: " << test_case.description;

	char* rendered = grammatica_grammar_render(ctx, result, true, true);
	ASSERT_NE(rendered, nullptr);
	EXPECT_STREQ(rendered, test_case.expected_render.c_str()) << "Description: " << test_case.description;

	grammatica_free_string(ctx, rendered);
	grammatica_grammar_destroy(ctx, result);
	grammatica_char_range_destroy(ctx, range);
}

INSTANTIATE_TEST_SUITE_P(
    CharRangeSimplifyTests, CharRangeSimplifyTest,
    ::testing::Values(
        CharRangeSimplifyTestCase{"Single character simplifies to String", {{{'a', 'a'}}}, false, GRAMMAR_TYPE_STRING, "\"a\""},
        CharRangeSimplifyTestCase{"CharRange stays as CharRange", {{{'a', 'z'}}}, false, GRAMMAR_TYPE_CHAR_RANGE, "[a-z]"},
        CharRangeSimplifyTestCase{
            "Multiple ranges stay as CharRange", {{{'a', 'c'}, {'x', 'z'}}}, false, GRAMMAR_TYPE_CHAR_RANGE, "[a-cx-z]"},
        CharRangeSimplifyTestCase{"Overlapping ranges merge", {{{'a', 'c'}, {'b', 'e'}}}, false, GRAMMAR_TYPE_CHAR_RANGE, "[a-e]"}));

/* Test cases for CharRange render */
struct CharRangeRenderTestCase {
	std::string description;
	std::vector<CharRangePair> ranges;
	bool negate;
	std::string expected;
};

class CharRangeRenderTest : public CharRangeTest, public ::testing::WithParamInterface<CharRangeRenderTestCase> {};

TEST_P(CharRangeRenderTest, Render) {
	auto test_case = GetParam();

	CharRange* range = grammatica_char_range_create(ctx, test_case.ranges.data(), test_case.ranges.size(), test_case.negate);
	ASSERT_NE(range, nullptr);

	char* result = grammatica_char_range_render(ctx, range, true, true);
	ASSERT_NE(result, nullptr);
	EXPECT_STREQ(result, test_case.expected.c_str()) << "Description: " << test_case.description;

	grammatica_free_string(ctx, result);
	grammatica_char_range_destroy(ctx, range);
}

INSTANTIATE_TEST_SUITE_P(CharRangeRenderTests, CharRangeRenderTest,
                         ::testing::Values(CharRangeRenderTestCase{"Simple range", {{{'a', 'z'}}}, false, "[a-z]"},
                                           CharRangeRenderTestCase{"Single character", {{{'a', 'a'}}}, false, "[a]"},
                                           CharRangeRenderTestCase{"Multiple ranges", {{{'a', 'z'}, {'0', '9'}}}, false, "[0-9a-z]"},
                                           CharRangeRenderTestCase{"Negated range", {{{'a', 'z'}}}, true, "[^a-z]"},
                                           CharRangeRenderTestCase{"Adjacent characters", {{{'a', 'b'}}}, false, "[ab]"},
                                           /* Test always safe characters (sample) */
                                           CharRangeRenderTestCase{"Always safe: digit", {{{'5', '5'}}}, false, "[5]"},
                                           CharRangeRenderTestCase{"Always safe: letter", {{{'g', 'g'}}}, false, "[g]"},
                                           CharRangeRenderTestCase{"Always safe: space", {{{' ', ' '}}}, false, "[ ]"},
                                           /* Test escape characters */
                                           CharRangeRenderTestCase{"Escape newline", {{{'\n', '\n'}}}, false, "[\\n]"},
                                           CharRangeRenderTestCase{"Escape carriage return", {{{'\r', '\r'}}}, false, "[\\r]"},
                                           CharRangeRenderTestCase{"Escape tab", {{{'\t', '\t'}}}, false, "[\\t]"},
                                           /* Test range-specific escapes */
                                           CharRangeRenderTestCase{"Escape caret", {{{'^', '^'}}}, false, "[\\^]"},
                                           CharRangeRenderTestCase{"Escape dash", {{{'-', '-'}}}, false, "[\\-]"},
                                           CharRangeRenderTestCase{"Escape left bracket", {{{'[', '['}}}, false, "[\\[]"},
                                           CharRangeRenderTestCase{"Escape right bracket", {{{']', ']'}}}, false, "[\\]]"},
                                           CharRangeRenderTestCase{"Escape backslash", {{{'\\', '\\'}}}, false, "[\\\\]"},
                                           /* Test hex escaping - note: \x00 (null) not supported due to C string limitations */
                                           CharRangeRenderTestCase{"Hex escape 0x01", {{{'\x01', '\x01'}}}, false, "[\\x01]"},
                                           CharRangeRenderTestCase{"Hex escape 0x7F", {{{'\x7F', '\x7F'}}}, false, "[\\x7F]"}));

/* Test as_string */
TEST_F(CharRangeTest, AsString) {
	CharRangePair ranges[] = {{'a', 'z'}};
	CharRange* range = grammatica_char_range_create(ctx, ranges, 1, false);
	ASSERT_NE(range, nullptr);

	char* result = grammatica_char_range_as_string(ctx, range);
	ASSERT_NE(result, nullptr);

	EXPECT_STREQ(result, "CharRange(char_ranges=[('a', 'z')], negate=False)");

	grammatica_free_string(ctx, result);
	grammatica_char_range_destroy(ctx, range);
}

/* Test equals */
TEST_F(CharRangeTest, Equals) {
	CharRangePair ranges1[] = {{'a', 'z'}};
	CharRangePair ranges2[] = {{'a', 'z'}};
	CharRangePair ranges3[] = {{'0', '9'}};

	CharRange* range1 = grammatica_char_range_create(ctx, ranges1, 1, false);
	CharRange* range2 = grammatica_char_range_create(ctx, ranges2, 1, false);
	CharRange* range3 = grammatica_char_range_create(ctx, ranges3, 1, false);

	ASSERT_NE(range1, nullptr);
	ASSERT_NE(range2, nullptr);
	ASSERT_NE(range3, nullptr);

	/* Same ranges should be equal */
	EXPECT_TRUE(grammatica_char_range_equals(ctx, range1, range2));

	/* Different ranges should not be equal */
	EXPECT_FALSE(grammatica_char_range_equals(ctx, range1, range3));

	/* Same object should equal itself */
	EXPECT_TRUE(grammatica_char_range_equals(ctx, range1, range1));

	grammatica_char_range_destroy(ctx, range1);
	grammatica_char_range_destroy(ctx, range2);
	grammatica_char_range_destroy(ctx, range3);
}

/* Test copy */
TEST_F(CharRangeTest, Copy) {
	CharRangePair ranges[] = {{'a', 'z'}};
	CharRange* original = grammatica_char_range_create(ctx, ranges, 1, false);
	ASSERT_NE(original, nullptr);

	CharRange* copy = grammatica_char_range_copy(ctx, original);
	ASSERT_NE(copy, nullptr);

	/* Copy should be equal */
	EXPECT_TRUE(grammatica_char_range_equals(ctx, original, copy));

	/* But should be different objects */
	EXPECT_NE(original, copy);

	grammatica_char_range_destroy(ctx, original);
	grammatica_char_range_destroy(ctx, copy);
}

/* Test get methods */
TEST_F(CharRangeTest, GetMethods) {
	CharRangePair ranges[] = {{'a', 'z'}, {'0', '9'}};
	CharRange* range = grammatica_char_range_create(ctx, ranges, 2, true);
	ASSERT_NE(range, nullptr);

	/* Test get_negate */
	EXPECT_TRUE(grammatica_char_range_get_negate(ctx, range));

	/* Test get_n */
	size_t ranges_n = grammatica_char_range_get_ranges_n(ctx, range);
	EXPECT_EQ(ranges_n, 2);

	/* Test get_ranges */
	CharRangePair out_ranges[10];
	int result = grammatica_char_range_get_ranges(ctx, range, out_ranges, 10);
	EXPECT_EQ(result, 2);

	grammatica_char_range_destroy(ctx, range);
}
