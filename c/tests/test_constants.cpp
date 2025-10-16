#include <gtest/gtest.h>

#include "grammatica_constants.h"

class ConstantsTest : public ::testing::Test {};

// Test ALWAYS_SAFE_CHARS
class AlwaysSafeCharsTest : public ::testing::TestWithParam<std::pair<char, bool>> {};

TEST_P(AlwaysSafeCharsTest, CharIsAlwaysSafe) {
	auto [ch, expected] = GetParam();
	EXPECT_EQ(char_is_always_safe(ch), expected);
}

INSTANTIATE_TEST_SUITE_P(AlwaysSafeCharsTests, AlwaysSafeCharsTest,
                         ::testing::Values(
                             // Digits
                             std::make_pair('0', true), std::make_pair('5', true), std::make_pair('9', true),
                             // Lowercase letters
                             std::make_pair('a', true), std::make_pair('m', true), std::make_pair('z', true),
                             // Uppercase letters
                             std::make_pair('A', true), std::make_pair('M', true), std::make_pair('Z', true),
                             // Space
                             std::make_pair(' ', true),
                             // Punctuation
                             std::make_pair('!', true), std::make_pair('#', true), std::make_pair('$', true), std::make_pair('%', true),
                             std::make_pair('&', true), std::make_pair('\'', true), std::make_pair('(', true), std::make_pair(')', true),
                             std::make_pair('*', true), std::make_pair('+', true), std::make_pair(',', true), std::make_pair('-', true),
                             std::make_pair('.', true), std::make_pair('/', true), std::make_pair(':', true), std::make_pair(';', true),
                             std::make_pair('<', true), std::make_pair('=', true), std::make_pair('>', true), std::make_pair('?', true),
                             std::make_pair('@', true), std::make_pair('[', true), std::make_pair(']', true), std::make_pair('^', true),
                             std::make_pair('_', true), std::make_pair('`', true), std::make_pair('{', true), std::make_pair('|', true),
                             std::make_pair('}', true), std::make_pair('~', true),
                             // Not safe
                             std::make_pair('\n', false), std::make_pair('\r', false), std::make_pair('\t', false),
                             std::make_pair('"', false), std::make_pair('\\', false), std::make_pair('\x80', false)));

// Test CHAR_ESCAPE_MAP
class CharEscapeMapTest : public ::testing::TestWithParam<std::pair<char, const char*>> {};

TEST_P(CharEscapeMapTest, CharGetEscape) {
	auto [ch, expected] = GetParam();
	const char* result = char_get_escape(ch);
	ASSERT_NE(result, nullptr);
	EXPECT_STREQ(result, expected);
}

INSTANTIATE_TEST_SUITE_P(CharEscapeMapTests, CharEscapeMapTest,
                         ::testing::Values(std::make_pair('\n', "\\n"), std::make_pair('\r', "\\r"), std::make_pair('\t', "\\t")));

// Test chars that should NOT be in CHAR_ESCAPE_MAP
class CharNoEscapeTest : public ::testing::TestWithParam<char> {};

TEST_P(CharNoEscapeTest, CharGetEscapeReturnsNull) {
	char ch = GetParam();
	const char* result = char_get_escape(ch);
	EXPECT_EQ(result, nullptr);
}

INSTANTIATE_TEST_SUITE_P(CharNoEscapeTests, CharNoEscapeTest, ::testing::Values('a', ' ', '0', '"', '\\', '^', '-', '[', ']'));

// Test STRING_LITERAL_ESCAPE_CHARS
class StringLiteralEscapeTest : public ::testing::TestWithParam<std::pair<char, bool>> {};

TEST_P(StringLiteralEscapeTest, CharIsStringLiteralEscape) {
	auto [ch, expected] = GetParam();
	EXPECT_EQ(char_is_string_literal_escape(ch), expected);
}

INSTANTIATE_TEST_SUITE_P(StringLiteralEscapeTests, StringLiteralEscapeTest,
                         ::testing::Values(std::make_pair('"', true), std::make_pair('\\', true), std::make_pair('a', false),
                                           std::make_pair(' ', false), std::make_pair('\n', false)));

// Test RANGE_ESCAPE_CHARS
class RangeEscapeTest : public ::testing::TestWithParam<std::pair<char, bool>> {};

TEST_P(RangeEscapeTest, CharIsRangeEscape) {
	auto [ch, expected] = GetParam();
	EXPECT_EQ(char_is_range_escape(ch), expected);
}

INSTANTIATE_TEST_SUITE_P(RangeEscapeTests, RangeEscapeTest,
                         ::testing::Values(std::make_pair('^', true), std::make_pair('-', true), std::make_pair('[', true),
                                           std::make_pair(']', true), std::make_pair('\\', true), std::make_pair('a', false),
                                           std::make_pair(' ', false), std::make_pair('"', false)));
