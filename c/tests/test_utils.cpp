#include <gtest/gtest.h>

#include <cstring>

#include "grammatica_utils.h"

class UtilsTest : public ::testing::Test {
   protected:
	void TearDown() override {
		/* Clean up any allocated strings */
	}
};

// Test char_to_cpoint
class CharToCpointTest : public ::testing::TestWithParam<std::pair<char, const char*>> {};

TEST_P(CharToCpointTest, CharToCpoint) {
	auto [ch, expected] = GetParam();
	char* result = char_to_cpoint(ch);
	ASSERT_NE(result, nullptr);
	EXPECT_STREQ(result, expected);
	free(result);
}

INSTANTIATE_TEST_SUITE_P(CharToCpointTests, CharToCpointTest,
                         ::testing::Values(std::make_pair('a', "\\u0061"), std::make_pair('A', "\\u0041"), std::make_pair('0', "\\u0030"),
                                           std::make_pair('\n', "\\u000A"),
                                           std::make_pair('\xE2', "\\u00E2")  // Note: Limited to single byte in C char
                                           ));

// Test ord_to_cpoint
class OrdToCpointTest : public ::testing::TestWithParam<std::pair<int, const char*>> {};

TEST_P(OrdToCpointTest, OrdToCpoint) {
	auto [ordinal, expected] = GetParam();
	char* result = ord_to_cpoint(ordinal);
	ASSERT_NE(result, nullptr);
	EXPECT_STREQ(result, expected);
	free(result);
}

INSTANTIATE_TEST_SUITE_P(OrdToCpointTests, OrdToCpointTest,
                         ::testing::Values(std::make_pair(97, "\\u0061"),         // 'a'
                                           std::make_pair(65, "\\u0041"),         // 'A'
                                           std::make_pair(10, "\\u000A"),         // newline
                                           std::make_pair(8364, "\\u20AC"),       // '€'
                                           std::make_pair(128512, "\\U0001F600")  // emoji (5+ digits)
                                           ));

// Test char_to_hex
class CharToHexTest : public ::testing::TestWithParam<std::pair<char, const char*>> {};

TEST_P(CharToHexTest, CharToHex) {
	auto [ch, expected] = GetParam();
	char* result = char_to_hex(ch);
	ASSERT_NE(result, nullptr);
	EXPECT_STREQ(result, expected);
	free(result);
}

INSTANTIATE_TEST_SUITE_P(CharToHexTests, CharToHexTest,
                         ::testing::Values(std::make_pair('a', "\\x61"), std::make_pair('A', "\\x41"), std::make_pair('0', "\\x30"),
                                           std::make_pair('\n', "\\x0A"), std::make_pair('~', "\\x7E")));

// Test ord_to_hex
class OrdToHexTest : public ::testing::TestWithParam<std::pair<int, const char*>> {};

TEST_P(OrdToHexTest, OrdToHex) {
	auto [ordinal, expected] = GetParam();
	char* result = ord_to_hex(ordinal);
	ASSERT_NE(result, nullptr);
	EXPECT_STREQ(result, expected);
	free(result);
}

INSTANTIATE_TEST_SUITE_P(OrdToHexTests, OrdToHexTest,
                         ::testing::Values(std::make_pair(97, "\\x61"),        // 'a'
                                           std::make_pair(65, "\\x41"),        // 'A'
                                           std::make_pair(10, "\\x0A"),        // newline
                                           std::make_pair(126, "\\x7E"),       // '~'
                                           std::make_pair(255, "\\xFF"),       // max 2-digit hex
                                           std::make_pair(128512, "\\x1F600")  // emoji (5+ digits)
                                           ));
