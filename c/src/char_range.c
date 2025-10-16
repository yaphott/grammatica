#include "grammatica.h"
#include "grammatica_internal.h"
#include "grammatica_constants.h"
#include "grammatica_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Helper function to compare CharRangePairs for qsort */
static int char_range_pair_compare(const void* a, const void* b) {
    const CharRangePair* pair_a = (const CharRangePair*)a;
    const CharRangePair* pair_b = (const CharRangePair*)b;
    return (unsigned char)pair_a->start - (unsigned char)pair_b->start;
}

/* Helper function to merge overlapping/adjacent ranges */
static size_t merge_char_ranges(CharRangePair* ranges, size_t num_ranges) {
    if (num_ranges <= 1) {
        return num_ranges;
    }
    qsort(ranges, num_ranges, sizeof(CharRangePair), char_range_pair_compare);
    size_t write_idx = 0;
    for (size_t i = 1; i < num_ranges; i++) {
        if ((unsigned char)ranges[i].start <= (unsigned char)ranges[write_idx].end + 1) {
            if ((unsigned char)ranges[i].end > (unsigned char)ranges[write_idx].end) {
                ranges[write_idx].end = ranges[i].end;
            }
        } else {
            write_idx++;
            ranges[write_idx] = ranges[i];
        }
    }
    return write_idx + 1;
}

/* Helper function to create set of ordinals from ranges */
static void ranges_to_ords(const CharRangePair* ranges, size_t num_ranges, bool* ord_set) {
    memset(ord_set, 0, 256 * sizeof(bool));
    for (size_t i = 0; i < num_ranges; i++) {
        for (unsigned char c = (unsigned char)ranges[i].start; c <= (unsigned char)ranges[i].end; c++) {
            ord_set[c] = true;
            if (c == (unsigned char)ranges[i].end) break;
        }
    }
}

/* Helper function to create ranges from ordinal set */
static size_t ords_to_ranges(const bool* ord_set, CharRangePair* out_ranges) {
    size_t count = 0;
    int start = -1;
    for (int i = 0; i < 256; i++) {
        if (ord_set[i]) {
            if (start == -1) {
                start = i;
            }
        } else {
            if (start != -1) {
                out_ranges[count].start = (char)start;
                out_ranges[count].end = (char)(i - 1);
                count++;
                start = -1;
            }
        }
    }
    if (start != -1) {
        out_ranges[count].start = (char)start;
        out_ranges[count].end = (char)255;
        count++;
    }
    return count;
}

/* Create CharRange from ranges */
CharRange* grammatica_char_range_create(GrammaticaContextHandle_t ctx, const CharRangePair* ranges, size_t num_ranges, bool negate) {
    if (!ctx || !ranges || num_ranges == 0) {
        if (ctx && num_ranges == 0) {
            grammatica_report_error(ctx, "char_ranges must not be empty");
        }
        return NULL;
    }
    /* Validate ranges */
    for (size_t i = 0; i < num_ranges; i++) {
        if ((unsigned char)ranges[i].end < (unsigned char)ranges[i].start) {
            grammatica_report_error(ctx, "end must be greater than or equal to start");
            return NULL;
        }
    }
    /* Convert to ordinal set and back to merge ranges */
    bool ord_set[256];
    ranges_to_ords(ranges, num_ranges, ord_set);
    CharRangePair* merged_ranges = (CharRangePair*)malloc(256 * sizeof(CharRangePair));
    if (!merged_ranges) {
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    size_t merged_count = ords_to_ranges(ord_set, merged_ranges);
    CharRange* range = (CharRange*)calloc(1, sizeof(CharRange));
    if (!range) {
        free(merged_ranges);
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    range->ranges = (CharRangePair*)realloc(merged_ranges, merged_count * sizeof(CharRangePair));
    if (!range->ranges && merged_count > 0) {
        range->ranges = merged_ranges;
    }
    range->num_ranges = merged_count;
    range->negate = negate;
    return range;
}

/* Create CharRange from characters */
CharRange* grammatica_char_range_from_chars(GrammaticaContextHandle_t ctx, const char* chars, size_t num_chars, bool negate) {
    if (!ctx || !chars || num_chars == 0) {
        if (ctx) {
            grammatica_report_error(ctx, "No characters provided");
        }
        return NULL;
    }
    /* Convert chars to ords */
    int* ords = (int*)malloc(num_chars * sizeof(int));
    if (!ords) {
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    for (size_t i = 0; i < num_chars; i++) {
        ords[i] = (unsigned char)chars[i];
    }
    CharRange* result = grammatica_char_range_from_ords(ctx, ords, num_chars, negate);
    free(ords);
    return result;
}

/* Create CharRange from ordinals */
CharRange* grammatica_char_range_from_ords(GrammaticaContextHandle_t ctx, const int* ords, size_t num_ords, bool negate) {
    if (!ctx || !ords || num_ords == 0) {
        if (ctx) {
            grammatica_report_error(ctx, "No ordinals provided");
        }
        return NULL;
    }
    /* Create ordinal set */
    bool ord_set[256];
    memset(ord_set, 0, sizeof(ord_set));
    for (size_t i = 0; i < num_ords; i++) {
        if (ords[i] >= 0 && ords[i] < 256) {
            ord_set[ords[i]] = true;
        }
    }
    /* Convert to ranges */
    CharRangePair* ranges = (CharRangePair*)malloc(256 * sizeof(CharRangePair));
    if (!ranges) {
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    size_t num_ranges = ords_to_ranges(ord_set, ranges);
    CharRange* result = grammatica_char_range_create(ctx, ranges, num_ranges, negate);
    free(ranges);
    return result;
}

/* Destroy CharRange */
void grammatica_char_range_destroy(GrammaticaContextHandle_t ctx, CharRange* range) {
    (void)ctx;
    if (!range) {
        return;
    }
    free(range->ranges);
    free(range);
}

/* Helper to escape a character for range context */
static char* escape_char_for_range(char c) {
    const char* special_escape = char_get_escape(c);
    if (special_escape) {
        return strdup(special_escape);
    }
    if (char_is_range_escape(c)) {
        char* result = (char*)malloc(3);
        if (result) {
            result[0] = '\\';
            result[1] = c;
            result[2] = '\0';
        }
        return result;
    }
    if (char_is_always_safe(c)) {
        char* result = (char*)malloc(2);
        if (result) {
            result[0] = c;
            result[1] = '\0';
        }
        return result;
    }
    return char_to_hex(c);
}

/* Render CharRange */
char* grammatica_char_range_render(GrammaticaContextHandle_t ctx, const CharRange* range, bool full, bool wrap) {
    (void)full;
    (void)wrap;
    if (!ctx || !range) {
        return NULL;
    }
    if (range->num_ranges == 0) {
        return NULL;
    }
    /* Convert to ordinal set for rendering */
    bool ord_set[256];
    ranges_to_ords(range->ranges, range->num_ranges, ord_set);
    /* Convert back to ranges (already merged) */
    CharRangePair* render_ranges = (CharRangePair*)malloc(256 * sizeof(CharRangePair));
    if (!render_ranges) {
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    size_t num_render_ranges = ords_to_ranges(ord_set, render_ranges);
    /* Build the string */
    char* result = (char*)malloc(4096);
    if (!result) {
        free(render_ranges);
        grammatica_report_error(ctx, "Memory allocation failed");
        return NULL;
    }
    size_t pos = 0;
    result[pos++] = '[';
    if (range->negate) {
        result[pos++] = '^';
    }
    for (size_t i = 0; i < num_render_ranges; i++) {
        char* start_esc = escape_char_for_range(render_ranges[i].start);
        if (!start_esc) {
            free(result);
            free(render_ranges);
            return NULL;
        }
        if (render_ranges[i].start == render_ranges[i].end) {
            pos += snprintf(result + pos, 4096 - pos, "%s", start_esc);
        } else {
            char* end_esc = escape_char_for_range(render_ranges[i].end);
            if (!end_esc) {
                free(start_esc);
                free(result);
                free(render_ranges);
                return NULL;
            }
            if ((unsigned char)render_ranges[i].end == (unsigned char)render_ranges[i].start + 1) {
                pos += snprintf(result + pos, 4096 - pos, "%s%s", start_esc, end_esc);
            } else {
                pos += snprintf(result + pos, 4096 - pos, "%s-%s", start_esc, end_esc);
            }
            free(end_esc);
        }
        free(start_esc);
    }
    result[pos++] = ']';
    result[pos] = '\0';
    free(render_ranges);
    char* final_result = strdup(result);
    free(result);
    return final_result;
}

/* Simplify CharRange */
Grammar* grammatica_char_range_simplify(GrammaticaContextHandle_t ctx, const CharRange* range) {
    if (!ctx || !range) {
        return NULL;
    }
    if (range->num_ranges == 0) {
        return NULL;
    }
    if (range->num_ranges == 1 && range->ranges[0].start == range->ranges[0].end) {
        /* Single character - convert to String */
        char value[2] = {range->ranges[0].start, '\0'};
        String* str = grammatica_string_create(ctx, value);
        if (!str) {
            return NULL;
        }
        Grammar* grammar = (Grammar*)malloc(sizeof(Grammar));
        if (!grammar) {
            grammatica_string_destroy(ctx, str);
            return NULL;
        }
        grammar->type = GRAMMAR_TYPE_STRING;
        grammar->data = str;
        return grammar;
    }
    /* Return a copy */
    CharRange* copy = grammatica_char_range_copy(ctx, range);
    if (!copy) {
        return NULL;
    }
    Grammar* grammar = (Grammar*)malloc(sizeof(Grammar));
    if (!grammar) {
        grammatica_char_range_destroy(ctx, copy);
        return NULL;
    }
    grammar->type = GRAMMAR_TYPE_CHAR_RANGE;
    grammar->data = copy;
    return grammar;
}

/* Convert CharRange to string representation */
char* grammatica_char_range_as_string(GrammaticaContextHandle_t ctx, const CharRange* range) {
    if (!ctx || !range) {
        return NULL;
    }
    char* result = (char*)malloc(4096);
    if (!result) {
        return NULL;
    }
    size_t pos = 0;
    pos += snprintf(result + pos, 4096 - pos, "CharRange(char_ranges=[");
    for (size_t i = 0; i < range->num_ranges; i++) {
        if (i > 0) {
            pos += snprintf(result + pos, 4096 - pos, ", ");
        }
        pos += snprintf(result + pos, 4096 - pos, "('%c', '%c')", range->ranges[i].start, range->ranges[i].end);
    }
    pos += snprintf(result + pos, 4096 - pos, "], negate=%s)", range->negate ? "True" : "False");
    char* final_result = strdup(result);
    free(result);
    return final_result;
}

/* Check if two CharRanges are equal */
bool grammatica_char_range_equals(GrammaticaContextHandle_t ctx, const CharRange* a, const CharRange* b) {
    if (!ctx) {
        return false;
    }
    if (a == b) {
        return true;
    }
    if (!a || !b) {
        return false;
    }
    if (a->negate != b->negate || a->num_ranges != b->num_ranges) {
        return false;
    }
    for (size_t i = 0; i < a->num_ranges; i++) {
        if (a->ranges[i].start != b->ranges[i].start || a->ranges[i].end != b->ranges[i].end) {
            return false;
        }
    }
    return true;
}

/* Copy CharRange */
CharRange* grammatica_char_range_copy(GrammaticaContextHandle_t ctx, const CharRange* range) {
    if (!ctx || !range) {
        return NULL;
    }
    return grammatica_char_range_create(ctx, range->ranges, range->num_ranges, range->negate);
}

/* Get number of ranges */
size_t grammatica_char_range_get_num_ranges(GrammaticaContextHandle_t ctx, const CharRange* range) {
    if (!ctx || !range) {
        return 0;
    }
    return range->num_ranges;
}

/* Get ranges */
int grammatica_char_range_get_ranges(GrammaticaContextHandle_t ctx, const CharRange* range, CharRangePair* out_ranges, size_t max_ranges) {
    if (!ctx || !range || !out_ranges) {
        return -1;
    }
    size_t copy_count = range->num_ranges < max_ranges ? range->num_ranges : max_ranges;
    memcpy(out_ranges, range->ranges, copy_count * sizeof(CharRangePair));
    return (int)copy_count;
}

/* Get negate flag */
bool grammatica_char_range_get_negate(GrammaticaContextHandle_t ctx, const CharRange* range) {
    if (!ctx || !range) {
        return false;
    }
    return range->negate;
}
