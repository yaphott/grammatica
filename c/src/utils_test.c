#include <gtest/gtest.h>
#include <grammatica.h>

class UtilsTest : public ::testing::Test {
protected:
    char buffer[64];
};

TEST_F(UtilsTest, CharToCpoint_LowerCase) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToCpoint('a', buffer, sizeof(buffer)));
    EXPECT_STREQ("\\u0061", buffer);
}

TEST_F(UtilsTest, CharToCpoint_UpperCase) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToCpoint('A', buffer, sizeof(buffer)));
    EXPECT_STREQ("\\u0041", buffer);
}

TEST_F(UtilsTest, CharToCpoint_Digit) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToCpoint('0', buffer, sizeof(buffer)));
    EXPECT_STREQ("\\u0030", buffer);
}

TEST_F(UtilsTest, CharToCpoint_Newline) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToCpoint('\n', buffer, sizeof(buffer)));
    EXPECT_STREQ("\\u000A", buffer);
}

TEST_F(UtilsTest, CharToCpoint_Euro) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToCpoint(0x20AC, buffer, sizeof(buffer)));
    EXPECT_STREQ("\\u20AC", buffer);
}

TEST_F(UtilsTest, CharToCpoint_Emoji) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToCpoint(0x1F600, buffer, sizeof(buffer)));
    EXPECT_STREQ("\\U0001F600", buffer);
}

TEST_F(UtilsTest, OrdToCpoint) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_ordToCpoint(97, buffer, sizeof(buffer)));
    EXPECT_STREQ("\\u0061", buffer);

    ASSERT_EQ(GRAMMATICA_OK, Grammatica_ordToCpoint(128512, buffer, sizeof(buffer)));
    EXPECT_STREQ("\\U0001F600", buffer);
}

TEST_F(UtilsTest, CharToHex) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToHex('a', buffer, sizeof(buffer)));
    EXPECT_STREQ("\\x61", buffer);

    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToHex('A', buffer, sizeof(buffer)));
    EXPECT_STREQ("\\x41", buffer);

    ASSERT_EQ(GRAMMATICA_OK, Grammatica_charToHex('\n', buffer, sizeof(buffer)));
    EXPECT_STREQ("\\x0A", buffer);
}

TEST_F(UtilsTest, OrdToHex) {
    ASSERT_EQ(GRAMMATICA_OK, Grammatica_ordToHex(97, buffer, sizeof(buffer)));
    EXPECT_STREQ("\\x61", buffer);

    ASSERT_EQ(GRAMMATICA_OK, Grammatica_ordToHex(255, buffer, sizeof(buffer)));
    EXPECT_STREQ("\\xFF", buffer);
}

TEST_F(UtilsTest, BufferTooSmall) {
    char small_buffer[2];
    EXPECT_EQ(GRAMMATICA_ERROR_BUFFER_TOO_SMALL, 
              Grammatica_charToCpoint('a', small_buffer, sizeof(small_buffer)));

    EXPECT_EQ(GRAMMATICA_ERROR_BUFFER_TOO_SMALL,
              Grammatica_charToHex('a', small_buffer, sizeof(small_buffer)));
}

TEST_F(UtilsTest, NullPointer) {
    EXPECT_EQ(GRAMMATICA_ERROR_NULL_POINTER, Grammatica_charToCpoint('a', NULL, 64));
    EXPECT_EQ(GRAMMATICA_ERROR_NULL_POINTER, Grammatica_charToHex('a', NULL, 64));
}
