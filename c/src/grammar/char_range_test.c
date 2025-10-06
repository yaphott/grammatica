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
    GrammaticaCharRangeEntry range = {'a', 'z'};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    // Manually clear the ranges to simulate empty state
    // In Python: grammar.char_ranges = []
    // In C: We can't directly modify internal state, so we test that
    // an empty range creation returns NULL and renders as NULL
    grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
    grammar = nullptr;

    // Creating with 0 ranges should return NULL
    grammar = grammaticaCharRangeCreate(nullptr, 0, false);
    EXPECT_EQ(nullptr, grammar);

    // Rendering NULL should return NULL
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
// ALWAYS_SAFE_CHARS - CHAR_ESCAPE_MAP - RANGE_ESCAPE_CHARS
// This includes: digits (0-9), letters (a-zA-Z), space, and punctuation (!#$%&'()*+,./:;<=>?@_`{|}~)
// Note: " is in STRING_LITERAL_ESCAPE_CHARS but not in RANGE_ESCAPE_CHARS or CHAR_ESCAPE_MAP

class CharRangeRenderAlwaysSafeTest : public ::testing::TestWithParam<std::pair<uint32_t, const char*>> {
protected:
    GrammaticaCharRange* grammar = nullptr;

    void TearDown() override {
        if (grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
        }
    }
};

TEST_P(CharRangeRenderAlwaysSafeTest, RenderAlwaysSafe) {
    auto [codepoint, expected] = GetParam();
    GrammaticaCharRangeEntry range = {codepoint, codepoint};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ(expected, rendered);
    free(rendered);
}

INSTANTIATE_TEST_SUITE_P(
    AlwaysSafeChars,
    CharRangeRenderAlwaysSafeTest,
    ::testing::Values(
        // Digits
        std::make_pair('0', "[0]"),
        std::make_pair('1', "[1]"),
        std::make_pair('2', "[2]"),
        std::make_pair('3', "[3]"),
        std::make_pair('4', "[4]"),
        std::make_pair('5', "[5]"),
        std::make_pair('6', "[6]"),
        std::make_pair('7', "[7]"),
        std::make_pair('8', "[8]"),
        std::make_pair('9', "[9]"),
        // Lowercase letters
        std::make_pair('a', "[a]"),
        std::make_pair('b', "[b]"),
        std::make_pair('c', "[c]"),
        std::make_pair('d', "[d]"),
        std::make_pair('e', "[e]"),
        std::make_pair('f', "[f]"),
        std::make_pair('g', "[g]"),
        std::make_pair('h', "[h]"),
        std::make_pair('i', "[i]"),
        std::make_pair('j', "[j]"),
        std::make_pair('k', "[k]"),
        std::make_pair('l', "[l]"),
        std::make_pair('m', "[m]"),
        std::make_pair('n', "[n]"),
        std::make_pair('o', "[o]"),
        std::make_pair('p', "[p]"),
        std::make_pair('q', "[q]"),
        std::make_pair('r', "[r]"),
        std::make_pair('s', "[s]"),
        std::make_pair('t', "[t]"),
        std::make_pair('u', "[u]"),
        std::make_pair('v', "[v]"),
        std::make_pair('w', "[w]"),
        std::make_pair('x', "[x]"),
        std::make_pair('y', "[y]"),
        std::make_pair('z', "[z]"),
        // Uppercase letters
        std::make_pair('A', "[A]"),
        std::make_pair('B', "[B]"),
        std::make_pair('C', "[C]"),
        std::make_pair('D', "[D]"),
        std::make_pair('E', "[E]"),
        std::make_pair('F', "[F]"),
        std::make_pair('G', "[G]"),
        std::make_pair('H', "[H]"),
        std::make_pair('I', "[I]"),
        std::make_pair('J', "[J]"),
        std::make_pair('K', "[K]"),
        std::make_pair('L', "[L]"),
        std::make_pair('M', "[M]"),
        std::make_pair('N', "[N]"),
        std::make_pair('O', "[O]"),
        std::make_pair('P', "[P]"),
        std::make_pair('Q', "[Q]"),
        std::make_pair('R', "[R]"),
        std::make_pair('S', "[S]"),
        std::make_pair('T', "[T]"),
        std::make_pair('U', "[U]"),
        std::make_pair('V', "[V]"),
        std::make_pair('W', "[W]"),
        std::make_pair('X', "[X]"),
        std::make_pair('Y', "[Y]"),
        std::make_pair('Z', "[Z]"),
        // Punctuation (excluding RANGE_ESCAPE_CHARS: ^-[]\\ and CHAR_ESCAPE_MAP: \n\r\t)
        std::make_pair('!', "[!]"),
        std::make_pair('#', "[#]"),
        std::make_pair('$', "[$]"),
        std::make_pair('%', "[%]"),
        std::make_pair('&', "[&]"),
        std::make_pair('\'', "[']"),
        std::make_pair('(', "[(]"),
        std::make_pair(')', "[)]"),
        std::make_pair('*', "[*]"),
        std::make_pair('+', "[+]"),
        std::make_pair(',', "[,]"),
        std::make_pair('.', "[.]"),
        std::make_pair('/', "[/]"),
        std::make_pair(':', "[:]"),
        std::make_pair(';', "[;]"),
        std::make_pair('<', "[<]"),
        std::make_pair('=', "[=]"),
        std::make_pair('>', "[>]"),
        std::make_pair('?', "[?]"),
        std::make_pair('@', "[@]"),
        std::make_pair('_', "[_]"),
        std::make_pair('`', "[`]"),
        std::make_pair('{', "[{]"),
        std::make_pair('|', "[|]"),
        std::make_pair('}', "[}]"),
        std::make_pair('~', "[~]"),
        std::make_pair('"', "[\"]"),  // STRING_LITERAL_ESCAPE_CHAR but not RANGE_ESCAPE_CHAR
        // Space
        std::make_pair(' ', "[ ]")
    )
);

// Test: test_char_range_render - Characters with CHAR_ESCAPE_MAP escapes
// (but not in RANGE_ESCAPE_CHARS, which is all of them: \n, \r, \t)
class CharRangeRenderCharEscapeMapTest : public ::testing::TestWithParam<std::pair<uint32_t, const char*>> {
protected:
    GrammaticaCharRange* grammar = nullptr;

    void TearDown() override {
        if (grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
        }
    }
};

TEST_P(CharRangeRenderCharEscapeMapTest, RenderCharEscapeMap) {
    auto [codepoint, expected] = GetParam();
    GrammaticaCharRangeEntry range = {codepoint, codepoint};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ(expected, rendered);
    free(rendered);
}

INSTANTIATE_TEST_SUITE_P(
    CharEscapeMap,
    CharRangeRenderCharEscapeMapTest,
    ::testing::Values(
        std::make_pair('\n', "[\\n]"),
        std::make_pair('\r', "[\\r]"),
        std::make_pair('\t', "[\\t]")
    )
);

// Test: test_char_range_render - Characters with RANGE_ESCAPE_CHARS (simple backslash escape)
class CharRangeRenderRangeEscapeTest : public ::testing::TestWithParam<std::pair<uint32_t, const char*>> {
protected:
    GrammaticaCharRange* grammar = nullptr;

    void TearDown() override {
        if (grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
        }
    }
};

TEST_P(CharRangeRenderRangeEscapeTest, RenderRangeEscape) {
    auto [codepoint, expected] = GetParam();
    GrammaticaCharRangeEntry range = {codepoint, codepoint};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ(expected, rendered);
    free(rendered);
}

INSTANTIATE_TEST_SUITE_P(
    RangeEscapeChars,
    CharRangeRenderRangeEscapeTest,
    ::testing::Values(
        std::make_pair('^', "[\\^]"),
        std::make_pair('-', "[\\-]"),
        std::make_pair('[', "[\\[]"),
        std::make_pair(']', "[\\]]"),
        std::make_pair('\\', "[\\\\]")
    )
);

// Test: test_char_range_render - Other characters are fully escaped (hex)
class CharRangeRenderHexEscapeTest : public ::testing::TestWithParam<std::pair<uint32_t, const char*>> {
protected:
    GrammaticaCharRange* grammar = nullptr;

    void TearDown() override {
        if (grammar != nullptr) {
            grammaticaGrammarUnref((GrammaticaGrammar*)grammar);
        }
    }
};

TEST_P(CharRangeRenderHexEscapeTest, RenderHexEscape) {
    auto [codepoint, expected] = GetParam();
    GrammaticaCharRangeEntry range = {codepoint, codepoint};
    grammar = grammaticaCharRangeCreate(&range, 1, false);
    ASSERT_NE(nullptr, grammar);

    char* rendered = grammaticaGrammarRender((GrammaticaGrammar*)grammar, true, true);
    ASSERT_NE(nullptr, rendered);
    EXPECT_STREQ(expected, rendered);
    free(rendered);
}

INSTANTIATE_TEST_SUITE_P(
    HexEscapedChars,
    CharRangeRenderHexEscapeTest,
    ::testing::Values(
        std::make_pair(0x00, "[\\x00]"),
        std::make_pair(0x01, "[\\x01]"),
        std::make_pair(0x02, "[\\x02]"),
        std::make_pair(0x03, "[\\x03]"),
        std::make_pair(0x04, "[\\x04]"),
        std::make_pair(0x7F, "[\\x7F]"),
        std::make_pair(0x80, "[\\x80]"),
        std::make_pair(0x81, "[\\x81]"),
        std::make_pair(0xFF, "[\\xFF]")
    )
);


// Note: test_char_range_as_string and test_char_range_attrs_dict are not applicable to C
// These are Python-specific methods for string representation and attribute dictionaries
