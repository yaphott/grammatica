#include <gtest/gtest.h>
#include <grammatica.h>

TEST(ConstantsTest, IsAlwaysSafeChar_Digits) {
    for (char ch = '0'; ch <= '9'; ch++) {
        EXPECT_TRUE(grammaticaIsAlwaysSafeChar(ch)) << "Digit " << ch << " should be safe";
    }
}

TEST(ConstantsTest, IsAlwaysSafeChar_LowerCase) {
    for (char ch = 'a'; ch <= 'z'; ch++) {
        EXPECT_TRUE(grammaticaIsAlwaysSafeChar(ch)) << "Lowercase " << ch << " should be safe";
    }
}

TEST(ConstantsTest, IsAlwaysSafeChar_UpperCase) {
    for (char ch = 'A'; ch <= 'Z'; ch++) {
        EXPECT_TRUE(grammaticaIsAlwaysSafeChar(ch)) << "Uppercase " << ch << " should be safe";
    }
}

TEST(ConstantsTest, IsAlwaysSafeChar_Space) {
    EXPECT_TRUE(grammaticaIsAlwaysSafeChar(' '));
}

TEST(ConstantsTest, IsAlwaysSafeChar_Punctuation) {
    const char* punctuation = "!#$%&'()*+,-./:;<=>?@[]^_`{|}~";
    for (const char* p = punctuation; *p != '\0'; p++) {
        EXPECT_TRUE(grammaticaIsAlwaysSafeChar(*p)) << "Punctuation '" << *p << "' should be safe";
    }
}

TEST(ConstantsTest, IsAlwaysSafeChar_NotSafe) {
    EXPECT_FALSE(grammaticaIsAlwaysSafeChar('\n'));
    EXPECT_FALSE(grammaticaIsAlwaysSafeChar('\r'));
    EXPECT_FALSE(grammaticaIsAlwaysSafeChar('\t'));
    EXPECT_FALSE(grammaticaIsAlwaysSafeChar('"'));
    EXPECT_FALSE(grammaticaIsAlwaysSafeChar('\\'));
    EXPECT_FALSE(grammaticaIsAlwaysSafeChar(0x80));
}

TEST(ConstantsTest, GetCharEscape) {
    char buffer[3];

    EXPECT_TRUE(grammaticaGetCharEscape('\n', buffer));
    EXPECT_STREQ("\\n", buffer);

    EXPECT_TRUE(grammaticaGetCharEscape('\r', buffer));
    EXPECT_STREQ("\\r", buffer);

    EXPECT_TRUE(grammaticaGetCharEscape('\t', buffer));
    EXPECT_STREQ("\\t", buffer);

    EXPECT_FALSE(grammaticaGetCharEscape('a', buffer));
    EXPECT_FALSE(grammaticaGetCharEscape(' ', buffer));
}

TEST(ConstantsTest, IsStringLiteralEscapeChar) {
    EXPECT_TRUE(grammaticaIsStringLiteralEscapeChar('"'));
    EXPECT_TRUE(grammaticaIsStringLiteralEscapeChar('\\'));

    EXPECT_FALSE(grammaticaIsStringLiteralEscapeChar('a'));
    EXPECT_FALSE(grammaticaIsStringLiteralEscapeChar('['));
}

TEST(ConstantsTest, IsRangeEscapeChar) {
    EXPECT_TRUE(grammaticaIsRangeEscapeChar('^'));
    EXPECT_TRUE(grammaticaIsRangeEscapeChar('-'));
    EXPECT_TRUE(grammaticaIsRangeEscapeChar('['));
    EXPECT_TRUE(grammaticaIsRangeEscapeChar(']'));
    EXPECT_TRUE(grammaticaIsRangeEscapeChar('\\'));

    EXPECT_FALSE(grammaticaIsRangeEscapeChar('a'));
    EXPECT_FALSE(grammaticaIsRangeEscapeChar('"'));
}
