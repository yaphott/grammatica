// #include <gtest/gtest.h>
// 
// #include <cstring>
// 
// #include "grammatica_utils.h"
// 
// class UtilsTest : public ::testing::Test {
//    protected:
// 	void TearDown() override {
// 		/* Clean up any allocated strings */
// 	}
// };
// 
// class CharToHexTest : public ::testing::TestWithParam<std::pair<const char, const char*>> {};
// 
// TEST_P(CharToHexTest, CharToHex) {
// 	auto [c, expected] = GetParam();
//    char out[GRAMMATICA_HEX_BUFFER_SIZE];
//    int ret = char_to_hex(c, out, GRAMMATICA_HEX_BUFFER_SIZE);
//    ASSERT_EQ(ret, 0);
//    EXPECT_STREQ(out, expected);
// }
// 
// INSTANTIATE_TEST_SUITE_P(CharToHexTests,
//                          CharToHexTest,
//                          ::testing::Values(std::make_pair('a', "\\x61"),
//                                            std::make_pair('A', "\\x41"),
//                                            std::make_pair('0', "\\x30"),
//                                            std::make_pair('\n', "\\x0A"),
//                                            std::make_pair('~', "\\x7E")));
// 
