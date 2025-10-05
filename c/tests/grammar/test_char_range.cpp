#include <gtest/gtest.h>
#include <grammatica.h>
#include <string>
#include <vector>

class CharRangeTest : public ::testing::Test {
protected:
    GrammaticaCharRange* grammar = nullptr;

    void TearDown() override {
        if (grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
        }
    }
};

// Test: test_char_range_render_empty
TEST_F(CharRangeTest, RenderEmpty) {
    // In C, we can't create a CharRange with 0 ranges (it returns NULL)
    // So we test that creating with NULL or 0 count returns NULL
    grammar = grammaticaCharRangeCreate(nullptr, 0, false);
    EXPECT_EQ(nullptr, grammar);

    // Also test that render returns NULL for invalid input
    char* rendered = grammaticaGrammarRender(nullptr, true, true);
    EXPECT_EQ(nullptr, rendered);
}

// Test: test_char_range_from_chars
TEST_F(CharRangeTest, FromChars) {
    uint32_t chars[] = {'a', 'b', 'c', 'x', 'y', 'z'};
    grammar = grammaticaCharRangeFromChars(chars, 6, false);
    ASSERT_NE(nullptr, grammar);

    size_t count;
    const GrammaticaCharRangeEntry* ranges = grammaticaCharRangeGetRanges(grammar, &count);
    EXPECT_EQ(2, count);  // Should merge to [a-c] and [x-z]
    EXPECT_EQ('a', ranges[0].start);
    EXPECT_EQ('c', ranges[0].end);
    EXPECT_EQ('x', ranges[1].start);
    EXPECT_EQ('z', ranges[1].end);
}

// Test: test_char_range_from_chars_negated
TEST_F(CharRangeTest, FromCharsNegated) {
    uint32_t chars[] = {'x', 'y', 'z'};
    grammar = grammaticaCharRangeFromChars(chars, 3, true);
    ASSERT_NE(nullptr, grammar);

    size_t count;
    const GrammaticaCharRangeEntry* ranges = grammaticaCharRangeGetRanges(grammar, &count);
    EXPECT_EQ(1, count);
    EXPECT_EQ('x', ranges[0].start);
    EXPECT_EQ('z', ranges[0].end);
    EXPECT_TRUE(grammaticaCharRangeIsNegated(grammar));
}

// Test: test_char_range_from_ords (same as from_chars in C)
TEST_F(CharRangeTest, FromOrds) {
    uint32_t ords[] = {97, 98, 99, 120, 121, 122};  // a, b, c, x, y, z
    grammar = grammaticaCharRangeFromChars(ords, 6, false);
    ASSERT_NE(nullptr, grammar);

    size_t count;
    const GrammaticaCharRangeEntry* ranges = grammaticaCharRangeGetRanges(grammar, &count);
    EXPECT_EQ(2, count);
    EXPECT_EQ('a', ranges[0].start);
    EXPECT_EQ('c', ranges[0].end);
    EXPECT_EQ('x', ranges[1].start);
    EXPECT_EQ('z', ranges[1].end);
}

// Test: test_char_range_from_ords_negated
TEST_F(CharRangeTest, FromOrdsNegated) {
    uint32_t ords[] = {97, 98, 99, 120, 121, 122};  // a, b, c, x, y, z
    grammar = grammaticaCharRangeFromChars(ords, 6, true);
    ASSERT_NE(nullptr, grammar);

    size_t count;
    const GrammaticaCharRangeEntry* ranges = grammaticaCharRangeGetRanges(grammar, &count);
    EXPECT_EQ(2, count);
    EXPECT_EQ('a', ranges[0].start);
    EXPECT_EQ('c', ranges[0].end);
    EXPECT_EQ('x', ranges[1].start);
    EXPECT_EQ('z', ranges[1].end);
    EXPECT_TRUE(grammaticaCharRangeIsNegated(grammar));
}

// Test: test_char_range_validation_empty
TEST_F(CharRangeTest, ValidationEmpty) {
    // Empty ranges should return NULL
    grammar = grammaticaCharRangeCreate(nullptr, 0, false);
    EXPECT_EQ(nullptr, grammar);

    GrammaticaCharRangeEntry ranges[] = {{'a', 'z'}};
    grammar = grammaticaCharRangeCreate(ranges, 0, false);
    EXPECT_EQ(nullptr, grammar);
}

// Test: test_char_range_validation_start_length
// In C, char length validation is implicit (we use uint32_t, not strings)
// This test doesn't apply to C implementation

// Test: test_char_range_validation_end_length  
// In C, char length validation is implicit (we use uint32_t, not strings)
// This test doesn't apply to C implementation

// Test: test_char_range_validation_end_before_start
TEST_F(CharRangeTest, ValidationEndBeforeStart) {
    GrammaticaCharRangeEntry range = {'z', 'a'};  // Invalid: end < start
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    EXPECT_EQ(nullptr, grammar);
}

// Test: test_char_range_simplify - "Single character simplifies to String"
TEST_F(CharRangeTest, SimplifySingleCharToString) {
    GrammaticaCharRangeEntry range = {'a', 'a'};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    // Should simplify to a String
    EXPECT_EQ(GRAMMATICA_TYPE_STRING, simplified->type);
    GrammaticaString* str = (GrammaticaString*)simplified;
    EXPECT_STREQ("a", grammaticaStringGetValue(str));

    grammaticaGrammarUnref(simplified);
}

// Test: test_char_range_simplify - "CharRange stays as CharRange"
TEST_F(CharRangeTest, SimplifyRangeStaysCharRange) {
    GrammaticaCharRangeEntry range = {'a', 'z'};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    // Should remain a CharRange
    EXPECT_EQ(GRAMMATICA_TYPE_CHAR_RANGE, simplified->type);

    grammaticaGrammarUnref(simplified);
}

// Test: test_char_range_simplify - "Multiple ranges stay as CharRange"
TEST_F(CharRangeTest, SimplifyMultipleRangesStayCharRange) {
    GrammaticaCharRangeEntry ranges[] = {{'a', 'c'}, {'x', 'z'}};
    grammar = grammaticaCharRangeCreate(ranges, 2, false);
    ASSERT_NE(nullptr, grammar);

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);

    // Should remain a CharRange
    EXPECT_EQ(GRAMMATICA_TYPE_CHAR_RANGE, simplified->type);

    grammaticaGrammarUnref(simplified);
}

// Test: test_char_range_simplify - "Overlapping ranges merge"
TEST_F(CharRangeTest, SimplifyOverlappingRangesMerge) {
    GrammaticaCharRangeEntry ranges[] = {{'a', 'c'}, {'b', 'e'}};
    grammar = grammaticaCharRangeCreate(ranges, 2, false);
    ASSERT_NE(nullptr, grammar);

    // Ranges should be merged during creation
    size_t count;
    const GrammaticaCharRangeEntry* merged = grammaticaCharRangeGetRanges(grammar, &count);
    EXPECT_EQ(1, count);
    EXPECT_EQ('a', merged[0].start);
    EXPECT_EQ('e', merged[0].end);

    GrammaticaGrammar* simplified = grammaticaGrammarSimplify((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, simplified);
    EXPECT_EQ(GRAMMATICA_TYPE_CHAR_RANGE, simplified->type);

    grammaticaGrammarUnref(simplified);
}

// Test: test_char_range_simplify_empty
TEST_F(CharRangeTest, SimplifyEmpty) {
    // Can't create an empty CharRange in C (returns NULL)
    // Test that simplify of NULL returns NULL
    GrammaticaGrammar* simplified = grammaticaGrammarSimplify(nullptr);
    EXPECT_EQ(nullptr, simplified);
}

// Test: test_char_range_render - "Simple range"
TEST_F(CharRangeTest, RenderSimpleRange) {
    GrammaticaCharRangeEntry range = {'a', 'z'};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("[a-z]", rendered);
    free(rendered);
}

// Test: test_char_range_render - "Single character"
TEST_F(CharRangeTest, RenderSingleChar) {
    GrammaticaCharRangeEntry range = {'a', 'a'};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("[a]", rendered);
    free(rendered);
}

// Test: test_char_range_render - "Multiple ranges"
TEST_F(CharRangeTest, RenderMultipleRanges) {
    GrammaticaCharRangeEntry ranges[] = {{'a', 'z'}, {'0', '9'}};
    grammar = grammaticaCharRangeCreate(ranges, 2, false);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("[0-9a-z]", rendered);  // Should be sorted
    free(rendered);
}

// Test: test_char_range_render - "Negated range"
TEST_F(CharRangeTest, RenderNegatedRange) {
    GrammaticaCharRangeEntry range = {'a', 'z'};
    grammar = grammaticaCharRangeCreate(&range, 1, true);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("[^a-z]", rendered);
    free(rendered);
}

// Test: test_char_range_render - "Adjacent characters"
TEST_F(CharRangeTest, RenderAdjacentChars) {
    GrammaticaCharRangeEntry range = {'a', 'b'};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ("[ab]", rendered);
    free(rendered);
}

// Test: test_char_range_render - Characters that are always safe in ranges
// Testing a sample of ALWAYS_SAFE_CHARS - CHAR_ESCAPE_MAP - RANGE_ESCAPE_CHARS
TEST_F(CharRangeTest, RenderAlwaysSafeChars) {
    // Test digit
    GrammaticaCharRangeEntry range1 = {'5', '5'};
    grammar = grammaticaCharRangeCreate(&range1, 1, false);
    char* rendered1 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[5]", rendered1);
    free(rendered1);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test letter
    GrammaticaCharRangeEntry range2 = {'g', 'g'};
    grammar = grammaticaCharRangeCreate(&range2, 1, false);
    char* rendered2 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[g]", rendered2);
    free(rendered2);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test punctuation (excluding range escape chars)
    GrammaticaCharRangeEntry range3 = {'!', '!'};
    grammar = grammaticaCharRangeCreate(&range3, 1, false);
    char* rendered3 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[!]", rendered3);
    free(rendered3);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test space
    GrammaticaCharRangeEntry range4 = {' ', ' '};
    grammar = grammaticaCharRangeCreate(&range4, 1, false);
    char* rendered4 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[ ]", rendered4);
    free(rendered4);
}

// Test: test_char_range_render - Characters with CHAR_ESCAPE_MAP escapes
TEST_F(CharRangeTest, RenderCharEscapeMap) {
    // Test newline
    GrammaticaCharRangeEntry range1 = {'\n', '\n'};
    grammar = grammaticaCharRangeCreate(&range1, 1, false);
    char* rendered1 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\n]", rendered1);
    free(rendered1);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test carriage return
    GrammaticaCharRangeEntry range2 = {'\r', '\r'};
    grammar = grammaticaCharRangeCreate(&range2, 1, false);
    char* rendered2 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\r]", rendered2);
    free(rendered2);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test tab
    GrammaticaCharRangeEntry range3 = {'\t', '\t'};
    grammar = grammaticaCharRangeCreate(&range3, 1, false);
    char* rendered3 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\t]", rendered3);
    free(rendered3);
}

// Test: test_char_range_render - Characters with RANGE_ESCAPE_CHARS (simple escape)
TEST_F(CharRangeTest, RenderRangeEscapeChars) {
    // Test caret
    GrammaticaCharRangeEntry range1 = {'^', '^'};
    grammar = grammaticaCharRangeCreate(&range1, 1, false);
    char* rendered1 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\^]", rendered1);
    free(rendered1);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test dash
    GrammaticaCharRangeEntry range2 = {'-', '-'};
    grammar = grammaticaCharRangeCreate(&range2, 1, false);
    char* rendered2 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\-]", rendered2);
    free(rendered2);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test left bracket
    GrammaticaCharRangeEntry range3 = {'[', '['};
    grammar = grammaticaCharRangeCreate(&range3, 1, false);
    char* rendered3 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\[]", rendered3);
    free(rendered3);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test right bracket
    GrammaticaCharRangeEntry range4 = {']', ']'};
    grammar = grammaticaCharRangeCreate(&range4, 1, false);
    char* rendered4 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\]]", rendered4);
    free(rendered4);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test backslash
    GrammaticaCharRangeEntry range5 = {'\\', '\\'};
    grammar = grammaticaCharRangeCreate(&range5, 1, false);
    char* rendered5 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\\\]", rendered5);
    free(rendered5);
}

// Test: test_char_range_render - Other characters are fully escaped (hex)
TEST_F(CharRangeTest, RenderOtherCharsHexEscaped) {
    // Test null character
    GrammaticaCharRangeEntry range1 = {0x00, 0x00};
    grammar = grammaticaCharRangeCreate(&range1, 1, false);
    char* rendered1 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\x00]", rendered1);
    free(rendered1);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test control character
    GrammaticaCharRangeEntry range2 = {0x01, 0x01};
    grammar = grammaticaCharRangeCreate(&range2, 1, false);
    char* rendered2 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\x01]", rendered2);
    free(rendered2);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test DEL character
    GrammaticaCharRangeEntry range3 = {0x7F, 0x7F};
    grammar = grammaticaCharRangeCreate(&range3, 1, false);
    char* rendered3 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\x7F]", rendered3);
    free(rendered3);
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Test high byte character
    GrammaticaCharRangeEntry range4 = {0xFF, 0xFF};
    grammar = grammaticaCharRangeCreate(&range4, 1, false);
    char* rendered4 = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    EXPECT_STREQ("[\\xFF]", rendered4);
    free(rendered4);
}

// Test: test_char_range_as_string (C doesn't have this method - informational only)
// This test doesn't apply to C implementation

// Test: test_char_range_attrs_dict (C doesn't have this method - informational only)
// This test doesn't apply to C implementation

// Additional C-specific tests

TEST_F(CharRangeTest, Copy) {
    GrammaticaCharRangeEntry range = {'a', 'z'};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    GrammaticaGrammar* copied = grammaticaGrammarCopy((GrammaticaGrammar*)grammar);
    ASSERT_NE(nullptr, copied);
    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar, copied, true));
    EXPECT_NE((GrammaticaGrammar*)grammar, copied);  // Different instances

    grammaticaGrammarUnref(copied);
}

TEST_F(CharRangeTest, Equals) {
    GrammaticaCharRangeEntry range1 = {'a', 'z'};
    grammar = grammaticaCharRangeCreate(&range1, 1, false);
    ASSERT_NE(nullptr, grammar);

    GrammaticaCharRangeEntry range2 = {'a', 'z'};
    GrammaticaCharRange* other = grammaticaCharRangeCreate(&range2, 1, false);
    ASSERT_NE(nullptr, other);

    GrammaticaCharRangeEntry range3 = {'0', '9'};
    GrammaticaCharRange* different = grammaticaCharRangeCreate(&range3, 1, false);
    ASSERT_NE(nullptr, different);

    EXPECT_TRUE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar, (GrammaticaGrammar*)other, true));
    EXPECT_FALSE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar, (GrammaticaGrammar*)different, true));

    grammaticaGrammarUnref((GrammaticaGrammar*)other);
    grammaticaGrammarUnref((GrammaticaGrammar*)different);
}

TEST_F(CharRangeTest, EqualsNegatedDifference) {
    GrammaticaCharRangeEntry range1 = {'a', 'z'};
    grammar = grammaticaCharRangeCreate(&range1, 1, false);
    ASSERT_NE(nullptr, grammar);

    GrammaticaCharRangeEntry range2 = {'a', 'z'};
    GrammaticaCharRange* negated = grammaticaCharRangeCreate(&range2, 1, true);
    ASSERT_NE(nullptr, negated);

    EXPECT_FALSE(grammaticaGrammarEquals((GrammaticaGrammar*)grammar, (GrammaticaGrammar*)negated, true));

    grammaticaGrammarUnref((GrammaticaGrammar*)negated);
}

TEST_F(CharRangeTest, MergeAdjacentRanges) {
    // Test that adjacent ranges are merged (e.g., a-c and d-f merge to a-f)
    GrammaticaCharRangeEntry ranges[] = {{'a', 'c'}, {'d', 'f'}};
    grammar = grammaticaCharRangeCreate(ranges, 2, false);
    ASSERT_NE(nullptr, grammar);

    size_t count;
    const GrammaticaCharRangeEntry* merged = grammaticaCharRangeGetRanges(grammar, &count);
    EXPECT_EQ(1, count);
    EXPECT_EQ('a', merged[0].start);
    EXPECT_EQ('f', merged[0].end);
}

TEST_F(CharRangeTest, GetRangesNullInput) {
    size_t count = 99;
    const GrammaticaCharRangeEntry* ranges = grammaticaCharRangeGetRanges(nullptr, &count);
    EXPECT_EQ(nullptr, ranges);
    EXPECT_EQ(0, count);
}

TEST_F(CharRangeTest, IsNegatedNullInput) {
    bool negated = grammaticaCharRangeIsNegated(nullptr);
    EXPECT_FALSE(negated);
}

