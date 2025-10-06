#include <gtest/gtest.h>
#include <grammatica.h>

class StringBuilderTest : public ::testing::Test {
protected:
    GrammaticaStringBuilder* sb = nullptr;
    void SetUp() override {
        sb = grammaticaStringBuilderCreate(0);
        ASSERT_NE(nullptr, sb);
    }
    void TearDown() override {
        if (sb != nullptr) {
            grammaticaStringBuilderDestroy(sb);
        }
    }
};

TEST_F(StringBuilderTest, CreateAndDestroy) {
    EXPECT_EQ(0, grammaticaStringBuilderLength(sb));
    EXPECT_STREQ("", grammaticaStringBuilderGet(sb));
}

TEST_F(StringBuilderTest, AppendString) {
    ASSERT_EQ(GRAMMATICA_OK, grammaticaStringBuilderAppend(sb, "Hello"));
    EXPECT_EQ(5, grammaticaStringBuilderLength(sb));
    EXPECT_STREQ("Hello", grammaticaStringBuilderGet(sb));

    ASSERT_EQ(GRAMMATICA_OK, grammaticaStringBuilderAppend(sb, " World"));
    EXPECT_EQ(11, grammaticaStringBuilderLength(sb));
    EXPECT_STREQ("Hello World", grammaticaStringBuilderGet(sb));
}

TEST_F(StringBuilderTest, AppendChar) {
    ASSERT_EQ(GRAMMATICA_OK, grammaticaStringBuilderAppend_char(sb, 'H'));
    ASSERT_EQ(GRAMMATICA_OK, grammaticaStringBuilderAppend_char(sb, 'i'));
    EXPECT_EQ(2, grammaticaStringBuilderLength(sb));
    EXPECT_STREQ("Hi", grammaticaStringBuilderGet(sb));
}

TEST_F(StringBuilderTest, AppendFormat) {
    ASSERT_EQ(GRAMMATICA_OK, grammaticaStringBuilderAppend_format(sb, "Number: %d", 42));
    EXPECT_STREQ("Number: 42", grammaticaStringBuilderGet(sb));

    ASSERT_EQ(GRAMMATICA_OK, grammaticaStringBuilderAppend_format(sb, ", String: %s", "test"));
    EXPECT_STREQ("Number: 42, String: test", grammaticaStringBuilderGet(sb));
}

TEST_F(StringBuilderTest, Clear) {
    grammaticaStringBuilderAppend(sb, "Hello");
    EXPECT_EQ(5, grammaticaStringBuilderLength(sb));

    grammaticaStringBuilderClear(sb);
    EXPECT_EQ(0, grammaticaStringBuilderLength(sb));
    EXPECT_STREQ("", grammaticaStringBuilderGet(sb));
}

TEST_F(StringBuilderTest, Extract) {
    grammaticaStringBuilderAppend(sb, "Test String");

    char* extracted = grammaticaStringBuilderExtract(sb);
    ASSERT_NE(nullptr, extracted);
    EXPECT_STREQ("Test String", extracted);

    // After extraction, the builder should be empty
    EXPECT_EQ(0, grammaticaStringBuilderLength(sb));

    free(extracted);
}

TEST_F(StringBuilderTest, LargeString) {
    // Test capacity growth
    for (int i = 0; i < 1000; i++) {
        ASSERT_EQ(GRAMMATICA_OK, grammaticaStringBuilderAppend(sb, "x"));
    }
    EXPECT_EQ(1000, grammaticaStringBuilderLength(sb));
}

TEST_F(StringBuilderTest, NullPointerChecks) {
    EXPECT_EQ(GRAMMATICA_ERROR_NULL_POINTER, grammaticaStringBuilderAppend(nullptr, "test"));
    EXPECT_EQ(GRAMMATICA_ERROR_NULL_POINTER, grammaticaStringBuilderAppend(sb, nullptr));
    EXPECT_EQ(GRAMMATICA_ERROR_NULL_POINTER, grammaticaStringBuilderAppend_char(nullptr, 'a'));
}
