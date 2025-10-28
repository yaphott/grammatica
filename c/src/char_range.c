#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "grammatica.h"
#include "grammatica_constants.h"
#include "grammatica_internal.h"
#include "grammatica_utils.h"

static int char_range_pair_compare(const void* a, const void* b) {
	const CharRangePair* pair_a = (const CharRangePair*)a;
	const CharRangePair* pair_b = (const CharRangePair*)b;
	return (char)pair_a->start - (char)pair_b->start;
}

static size_t merge_char_ranges(CharRangePair* ranges, size_t n) {
	if (n <= 1) {
		return n;
	}
	qsort(ranges, n, sizeof(CharRangePair), char_range_pair_compare);
	size_t write_idx = 0;
	for (size_t i = 1; i < n; i++) {
		if ((char)ranges[i].start <= (char)ranges[write_idx].end + 1) {
			if ((char)ranges[i].end > (char)ranges[write_idx].end) {
				ranges[write_idx].end = ranges[i].end;
			}
		} else {
			write_idx++;
			ranges[write_idx] = ranges[i];
		}
	}
	return write_idx + 1;
}

static void ranges_to_ords(const CharRangePair* ranges, size_t n, bool* ord_set) {
	memset(ord_set, 0, 256 * sizeof(bool));
	for (size_t i = 0; i < n; i++) {
		for (char c = (char)ranges[i].start; c <= (char)ranges[i].end; c++) {
			ord_set[c] = true;
			if (c == (char)ranges[i].end) {
				break;
			}
		}
	}
}

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

CharRange* grammatica_char_range_create(GrammaticaContextHandle_t ctx, const CharRangePair* ranges, size_t n, bool negate) {
	if (!ctx || !ranges || n == 0) {
		if (ctx && n == 0) {
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_INVALID_PARAMETER, "char_ranges must not be empty");
		}
		return NULL;
	}
	/* Validate ranges */
	for (size_t i = 0; i < n; i++) {
		if ((char)ranges[i].end < (char)ranges[i].start) {
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_INVALID_PARAMETER, "end must be greater than or equal to start");
			return NULL;
		}
	}
	/* Convert to ordinal set and back to merge ranges */
	bool ord_set[256];
	ranges_to_ords(ranges, n, ord_set);
	CharRangePair* merged_ranges = (CharRangePair*)malloc(256 * sizeof(CharRangePair));
	if (!merged_ranges) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	size_t merged_count = ords_to_ranges(ord_set, merged_ranges);
	CharRange* range = (CharRange*)calloc(1, sizeof(CharRange));
	if (!range) {
		free(merged_ranges);
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	range->ranges = (CharRangePair*)realloc(merged_ranges, merged_count * sizeof(CharRangePair));
	if (!range->ranges && merged_count > 0) {
		range->ranges = merged_ranges;
	}
	range->ranges_n = merged_count;
	range->negate = negate;
	return range;
}

CharRange* grammatica_char_range_from_chars(GrammaticaContextHandle_t ctx, const char* chars, size_t num_chars, bool negate) {
	if (!ctx || !chars || num_chars == 0) {
		if (ctx) {
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_INVALID_PARAMETER, "No characters provided");
		}
		return NULL;
	}
	/* Convert chars to ords */
	int* ords = (int*)malloc(num_chars * sizeof(int));
	if (!ords) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	for (size_t i = 0; i < num_chars; i++) {
		ords[i] = (char)chars[i];
	}
	CharRange* result = grammatica_char_range_from_ords(ctx, ords, num_chars, negate);
	free(ords);
	return result;
}

CharRange* grammatica_char_range_from_ords(GrammaticaContextHandle_t ctx, const int* ords, size_t num_ords, bool negate) {
	if (!ctx || !ords || num_ords == 0) {
		if (ctx) {
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_INVALID_PARAMETER, "No ordinals provided");
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
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	size_t n = ords_to_ranges(ord_set, ranges);
	CharRange* result = grammatica_char_range_create(ctx, ranges, n, negate);
	free(ranges);
	return result;
}

void grammatica_char_range_destroy(CharRange* range) {
	if (range == NULL) {
		return;
	}
	free(range->ranges);
	free(range);
}

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

char* grammatica_char_range_render(GrammaticaContextHandle_t ctx, const CharRange* range, bool full, bool wrap) {
	(void)full;
	(void)wrap;
	if (!ctx || !range) {
		return NULL;
	}
	if (range->ranges_n == 0) {
		return NULL;
	}
	/* Convert to ordinal set for rendering */
	bool ord_set[256];
	ranges_to_ords(range->ranges, range->ranges_n, ord_set);
	/* Convert back to ranges (already merged) */
	CharRangePair* render_ranges = (CharRangePair*)malloc(256 * sizeof(CharRangePair));
	if (!render_ranges) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		return NULL;
	}
	size_t render_ranges_n = ords_to_ranges(ord_set, render_ranges);
	/* Build the string - use stack buffer for small strings */
	char stack_buf[512];
	char* heap_buf = NULL;
	char* result;
	size_t buf_size = sizeof(stack_buf);
	size_t pos = 0;
	bool using_heap = false;

	result = stack_buf;
	result[pos++] = '[';
	if (range->negate) {
		result[pos++] = '^';
	}
	for (size_t i = 0; i < render_ranges_n; i++) {
		/* Check if we need more space (estimate) */
		if (pos + 32 > buf_size) {
			/* Switch to heap allocation if needed */
			if (!using_heap) {
				heap_buf = (char*)malloc(4096);
				if (!heap_buf) {
					free(render_ranges);
					grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
					return NULL;
				}
				memcpy(heap_buf, stack_buf, pos);
				result = heap_buf;
				buf_size = 4096;
				using_heap = true;
			}
		}

		char* start_esc = escape_char_for_range(render_ranges[i].start);
		if (!start_esc) {
			if (using_heap)
				free(heap_buf);
			free(render_ranges);
			return NULL;
		}
		if (render_ranges[i].start == render_ranges[i].end) {
			pos += snprintf(result + pos, buf_size - pos, "%s", start_esc);
		} else {
			char* end_esc = escape_char_for_range(render_ranges[i].end);
			if (!end_esc) {
				free(start_esc);
				if (using_heap)
					free(heap_buf);
				free(render_ranges);
				return NULL;
			}
			if ((char)render_ranges[i].end == (char)render_ranges[i].start + 1) {
				pos += snprintf(result + pos, buf_size - pos, "%s%s", start_esc, end_esc);
			} else {
				pos += snprintf(result + pos, buf_size - pos, "%s-%s", start_esc, end_esc);
			}
			free(end_esc);
		}
		free(start_esc);
	}
	result[pos++] = ']';
	result[pos] = '\0';
	free(render_ranges);

	/* Return heap buffer if we allocated one, otherwise duplicate stack buffer */
	if (using_heap) {
		return heap_buf;
	} else {
		return strdup(stack_buf);
	}
}

Grammar* grammatica_char_range_simplify(GrammaticaContextHandle_t ctx, const CharRange* range) {
	if (!ctx || !range) {
		return NULL;
	}
	if (range->ranges_n == 0) {
		return NULL;
	}

	Grammar* grammar = NULL;
	String* str = NULL;
	CharRange* copy = NULL;

	if (range->ranges_n == 1 && range->ranges[0].start == range->ranges[0].end) {
		/* Single character - convert to String */
		char value[2] = {range->ranges[0].start, '\0'};
		str = grammatica_string_create(ctx, value);
		if (!str) {
			goto cleanup;
		}
		grammar = (Grammar*)malloc(sizeof(Grammar));
		if (!grammar) {
			grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
			goto cleanup;
		}
		grammar->type = GRAMMAR_TYPE_STRING;
		grammar->data = str;
		return grammar; /* Success */
	}

	/* Return a copy */
	copy = grammatica_char_range_copy(ctx, range);
	if (!copy) {
		goto cleanup;
	}
	grammar = (Grammar*)malloc(sizeof(Grammar));
	if (!grammar) {
		grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
		goto cleanup;
	}
	grammar->type = GRAMMAR_TYPE_CHAR_RANGE;
	grammar->data = copy;
	return grammar; /* Success */

cleanup:
	if (str != NULL) {
		grammatica_string_destroy(str);
	}
	if (copy != NULL) {
		grammatica_char_range_destroy(copy);
	}
	if (grammar != NULL) {
		free(grammar);
	}
	return NULL;
}

char* grammatica_char_range_as_string(GrammaticaContextHandle_t ctx, const CharRange* range) {
	if (!ctx || !range) {
		return NULL;
	}
	/* Use stack buffer for small strings, heap for large */
	char stack_buf[512];
	char* heap_buf = NULL;
	char* result;
	size_t buf_size = sizeof(stack_buf);
	size_t pos = 0;
	bool using_heap = false;

	result = stack_buf;
	pos += snprintf(result + pos, buf_size - pos, "CharRange(char_ranges=[");
	for (size_t i = 0; i < range->ranges_n; i++) {
		/* Check if we need more space */
		if (pos + 64 > buf_size && !using_heap) {
			heap_buf = (char*)malloc(4096);
			if (!heap_buf) {
				grammatica_report_error_with_code(ctx, GRAMMATICA_ERROR_OUT_OF_MEMORY, "Memory allocation failed");
				return NULL;
			}
			memcpy(heap_buf, stack_buf, pos);
			result = heap_buf;
			buf_size = 4096;
			using_heap = true;
		}

		if (i > 0) {
			pos += snprintf(result + pos, buf_size - pos, ", ");
		}
		pos += snprintf(result + pos, buf_size - pos, "('%c', '%c')", range->ranges[i].start, range->ranges[i].end);
	}
	pos += snprintf(result + pos, buf_size - pos, "], negate=%s)", range->negate ? "True" : "False");

	/* Return heap buffer if we allocated one, otherwise duplicate stack buffer */
	if (using_heap) {
		return heap_buf;
	} else {
		return strdup(stack_buf);
	}
}

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
	if (a->negate != b->negate || a->ranges_n != b->ranges_n) {
		return false;
	}
	for (size_t i = 0; i < a->ranges_n; i++) {
		if (a->ranges[i].start != b->ranges[i].start || a->ranges[i].end != b->ranges[i].end) {
			return false;
		}
	}
	return true;
}

CharRange* grammatica_char_range_copy(GrammaticaContextHandle_t ctx, const CharRange* range) {
	if (!ctx || !range) {
		return NULL;
	}
	return grammatica_char_range_create(ctx, range->ranges, range->ranges_n, range->negate);
}

size_t grammatica_char_range_get_ranges_n(GrammaticaContextHandle_t ctx, const CharRange* range) {
	if (!ctx || !range) {
		return 0;
	}
	return range->ranges_n;
}

int grammatica_char_range_get_ranges(GrammaticaContextHandle_t ctx, const CharRange* range, CharRangePair* out_ranges, size_t max_ranges) {
	if (!ctx || !range || !out_ranges) {
		return -1;
	}
	size_t copy_count = range->ranges_n < max_ranges ? range->ranges_n : max_ranges;
	memcpy(out_ranges, range->ranges, copy_count * sizeof(CharRangePair));
	return (int)copy_count;
}

bool grammatica_char_range_get_negate(GrammaticaContextHandle_t ctx, const CharRange* range) {
	if (!ctx || !range) {
		return false;
	}
	return range->negate;
}
