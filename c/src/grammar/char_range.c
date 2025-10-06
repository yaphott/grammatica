#include "char_range.h"
#include "string.h"
#include "../constants.h"
#include "../utils.h"
#include <stdlib.h>
#include <string.h>

// Forward declarations
static void char_range_destroy(GrammaticaGrammar* grammar);
static char* char_range_render(const GrammaticaGrammar* grammar, bool full, bool wrap);
static GrammaticaGrammar* char_range_simplify(const GrammaticaGrammar* grammar);
static GrammaticaGrammar* char_range_copy(const GrammaticaGrammar* grammar);
static bool char_range_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier);
// Helper function to escape a character for range rendering
static void escape_char_for_range(uint32_t ch, GrammaticaStringBuilder* sb_local) {
    if (grammaticaIsRangeEscapeChar(ch)) {
        grammaticaStringBuilderAppend_char(sb_local, '\\');
        grammaticaStringBuilderAppend_char(sb_local, (char)ch);
    } else if (grammaticaIsAlwaysSafeChar(ch)) {
        grammaticaStringBuilderAppend_char(sb_local, (char)ch);
    } else {
        char escape_buf[3];
        if (grammaticaGetCharEscape(ch, escape_buf)) {
            grammaticaStringBuilderAppend(sb_local, escape_buf);
        } else {
            char hex_buf[16];
            if (Grammatica_charToHex(ch, hex_buf, sizeof(hex_buf)) == GRAMMATICA_OK) {
                grammaticaStringBuilderAppend(sb_local, hex_buf);
            }
        }
    }
}

static const GrammaticaGrammarVTable char_range_vtable = {
    .destroy = char_range_destroy,
    .render = char_range_render,
    .simplify = char_range_simplify,
    .copy = char_range_copy,
    .equals = char_range_equals,
};

// Helper function to compare char range entries for sorting
static int compare_range_entries(const void* a, const void* b) {
    const GrammaticaCharRangeEntry* ra = (const GrammaticaCharRangeEntry*)a;
    const GrammaticaCharRangeEntry* rb = (const GrammaticaCharRangeEntry*)b;
    if (ra->start < rb->start) {
        return -1;
    }
    if (ra->start > rb->start) {
        return 1;
    }
    return 0;
}

// Helper function to merge overlapping ranges
static size_t merge_ranges(GrammaticaCharRangeEntry* ranges, size_t count) {
    if (count <= 1) {
        return count;
    }
    // Sort ranges by start
    qsort(ranges, count, sizeof(GrammaticaCharRangeEntry), compare_range_entries);
    size_t write_pos = 0;
    for (size_t read_pos = 1; read_pos < count; read_pos++) {
        // Check if current range overlaps or is adjacent to the previous one
        if (ranges[read_pos].start <= ranges[write_pos].end + 1) {
            // Merge: extend the end if necessary
            if (ranges[read_pos].end > ranges[write_pos].end) {
                ranges[write_pos].end = ranges[read_pos].end;
            }
        } else {
            // No overlap, move to next position
            write_pos++;
            if (write_pos != read_pos) {
                ranges[write_pos] = ranges[read_pos];
            }
        }
    }
    return write_pos + 1;
}

GrammaticaCharRange* grammaticaCharRangeCreate(
    const GrammaticaCharRangeEntry* ranges,
    size_t rangeCount,
    bool negate
) {
    if (ranges == NULL || rangeCount == 0) {
        return NULL;
    }
    // Validate ranges
    for (size_t i = 0; i < rangeCount; i++) {
        if (ranges[i].end < ranges[i].start) {
            return NULL;
        }
    }
    GrammaticaCharRange* grammar = (GrammaticaCharRange*)malloc(sizeof(GrammaticaCharRange));
    if (grammar == NULL) {
        return NULL;
    }
    // Copy and merge ranges
    grammar->ranges = (GrammaticaCharRangeEntry*)malloc(rangeCount * sizeof(GrammaticaCharRangeEntry));
    if (grammar->ranges == NULL) {
        free(grammar);
        return NULL;
    }
    memcpy(grammar->ranges, ranges, rangeCount * sizeof(GrammaticaCharRangeEntry));
    grammar->rangeCount = merge_ranges(grammar->ranges, rangeCount);
    // Shrink allocation if ranges were merged
    if (grammar->rangeCount < rangeCount) {
        GrammaticaCharRangeEntry* new_ranges = (GrammaticaCharRangeEntry*)realloc(
            grammar->ranges,
            grammar->rangeCount * sizeof(GrammaticaCharRangeEntry)
        );
        if (new_ranges != NULL) {
            grammar->ranges = new_ranges;
        }
    }
    grammar->negate = negate;
    grammar->base.type = GRAMMATICA_TYPE_CHAR_RANGE;
    grammar->base.vtable = &char_range_vtable;
    atomic_init(&grammar->base.refCount, 1);
    return grammar;
}

GrammaticaCharRange* grammaticaCharRangeFromChars(
    const uint32_t* chars,
    size_t char_count,
    bool negate
) {
    if (chars == NULL || char_count == 0) {
        return NULL;
    }
    // Allocate ranges array (worst case: each char is a separate range)
    GrammaticaCharRangeEntry* ranges = (GrammaticaCharRangeEntry*)malloc(char_count * sizeof(GrammaticaCharRangeEntry));
    if (ranges == NULL) {
        return NULL;
    }
    // Convert chars to ranges
    for (size_t i = 0; i < char_count; i++) {
        ranges[i].start = chars[i];
        ranges[i].end = chars[i];
    }
    GrammaticaCharRange* result = grammaticaCharRangeCreate(ranges, char_count, negate);
    free(ranges);
    return result;
}

const GrammaticaCharRangeEntry* grammaticaCharRangeGetRanges(
    const GrammaticaCharRange* grammar,
    size_t* rangeCount
) {
    if (grammar == NULL) {
        if (rangeCount != NULL) {
            *rangeCount = 0;
        }
        return NULL;
    }
    if (rangeCount != NULL) {
        *rangeCount = grammar->rangeCount;
    }
    return grammar->ranges;
}

bool grammaticaCharRangeIsNegated(const GrammaticaCharRange* grammar) {
    if (grammar == NULL) {
        return false;
    }
    return grammar->negate;
}

static void char_range_destroy(GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return;
    }
    GrammaticaCharRange* char_range = (GrammaticaCharRange*)grammar;
    if (char_range->ranges != NULL) {
        free(char_range->ranges);
    }
    free(char_range);
}

static char* char_range_render(const GrammaticaGrammar* grammar, bool full, bool wrap) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaCharRange* char_range = (const GrammaticaCharRange*)grammar;
    if (char_range->rangeCount == 0) {
        return NULL;
    }
    GrammaticaStringBuilder* sb = grammaticaStringBuilderCreate(0);
    if (sb == NULL) {
        return NULL;
    }
    grammaticaStringBuilderAppend_char(sb, '[');
    if (char_range->negate) {
        grammaticaStringBuilderAppend_char(sb, '^');
    }
    for (size_t i = 0; i < char_range->rangeCount; i++) {
        uint32_t start = char_range->ranges[i].start;
        uint32_t end = char_range->ranges[i].end;
        if (start == end) {
            // Single character
            escape_char_for_range(start, sb);
        } else if (end == start + 1) {
            // Adjacent characters
            escape_char_for_range(start, sb);
            escape_char_for_range(end, sb);
        } else {
            // Range
            escape_char_for_range(start, sb);
            grammaticaStringBuilderAppend_char(sb, '-');
            escape_char_for_range(end, sb);
        }
    }
    grammaticaStringBuilderAppend_char(sb, ']');
    char* result = grammaticaStringBuilderExtract(sb);
    grammaticaStringBuilderDestroy(sb);
    return result;
}

static GrammaticaGrammar* char_range_simplify(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaCharRange* char_range = (const GrammaticaCharRange*)grammar;
    if (char_range->rangeCount == 0) {
        return NULL;
    }
    // If it's a single character range, convert to String
    if (char_range->rangeCount == 1 && 
        char_range->ranges[0].start == char_range->ranges[0].end &&
        !char_range->negate) {
        char ch[2] = {(char)char_range->ranges[0].start, '\0'};
        return (GrammaticaGrammar*)grammaticaStringCreate(ch);
    }
    // Otherwise, return a copy
    return (GrammaticaGrammar*)grammaticaCharRangeCreate(
        char_range->ranges,
        char_range->rangeCount,
        char_range->negate
    );
}

static GrammaticaGrammar* char_range_copy(const GrammaticaGrammar* grammar) {
    if (grammar == NULL) {
        return NULL;
    }
    const GrammaticaCharRange* char_range = (const GrammaticaCharRange*)grammar;
    return (GrammaticaGrammar*)grammaticaCharRangeCreate(
        char_range->ranges,
        char_range->rangeCount,
        char_range->negate
    );
}

static bool char_range_equals(const GrammaticaGrammar* grammar, const GrammaticaGrammar* other, bool check_quantifier) {
    if (grammar == other) {
        return true;
    }
    if (grammar == NULL || other == NULL) {
        return false;
    }
    if (grammar->type != other->type) {
        return false;
    }
    const GrammaticaCharRange* g1 = (const GrammaticaCharRange*)grammar;
    const GrammaticaCharRange* g2 = (const GrammaticaCharRange*)other;
    if (g1->negate != g2->negate) {
        return false;
    }
    if (g1->rangeCount != g2->rangeCount) {
        return false;
    }
    for (size_t i = 0; i < g1->rangeCount; i++) {
        if (g1->ranges[i].start != g2->ranges[i].start ||
            g1->ranges[i].end != g2->ranges[i].end) {
            return false;
        }
    }
    return true;
}
