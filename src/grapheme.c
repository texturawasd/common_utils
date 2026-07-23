#include "../grapheme.h"
#include "../simple_strings.h"

/* ------------------------------------------------------------------ */
/* Classification tables                                               */
/*                                                                      */
/* These are hand-picked ranges covering the combining marks and       */
/* control code points that matter in practice, not a generated dump   */
/* of the full Unicode character database. See the "not full UAX #29"  */
/* note in grapheme.h.                                                 */
/* ------------------------------------------------------------------ */

#define GRAPHEME_CLUSTER_ZWJ 0x200Du

struct codepoint_range {
    uint32_t lo, hi;
};

/* Code points that attach to whatever came before them (combining
 * marks: accents, diacritics, vowel signs, tone marks, emoji skin tone
 * modifiers, etc.) */
static const struct codepoint_range GRAPHEME_CLUSTER_EXTEND_RANGES[] = {
    {0x0300, 0x036F}, /* Combining Diacritical Marks */
    {0x0483, 0x0489}, /* Cyrillic combining marks */
    {0x0591, 0x05BD}, /* Hebrew points */
    {0x05BF, 0x05BF},
    {0x05C1, 0x05C2},
    {0x05C4, 0x05C5},
    {0x05C7, 0x05C7},
    {0x0610, 0x061A}, /* Arabic marks */
    {0x064B, 0x065F},
    {0x0670, 0x0670},
    {0x06D6, 0x06DC},
    {0x06DF, 0x06E4},
    {0x06E7, 0x06E8},
    {0x06EA, 0x06ED},
    {0x0711, 0x0711}, /* Syriac */
    {0x0730, 0x074A},
    {0x07A6, 0x07B0}, /* Thaana */
    {0x0900, 0x0902}, /* Devanagari */
    {0x093A, 0x093A},
    {0x093C, 0x093C},
    {0x0941, 0x0948},
    {0x094D, 0x094D},
    {0x0951, 0x0957},
    {0x0962, 0x0963},
    {0x0980, 0x0983}, /* Bengali (subset) */
    {0x09BC, 0x09BC},
    {0x0A01, 0x0A03}, /* Gurmukhi (subset) */
    {0x0E31, 0x0E31}, /* Thai */
    {0x0E34, 0x0E3A},
    {0x0E47, 0x0E4E},
    {0x0EB1, 0x0EB1}, /* Lao */
    {0x0EB4, 0x0EBC},
    {0x0EC8, 0x0ECD},
    {0x1AB0, 0x1AFF}, /* Combining Diacritical Marks Extended */
    {0x1DC0, 0x1DFF}, /* Combining Diacritical Marks Supplement */
    {0x200C, 0x200C}, /* Zero Width Non-Joiner */
    {0x20D0, 0x20FF}, /* Combining Diacritical Marks for Symbols */
    {0x302A, 0x302F}, /* CJK tone marks */
    {0x3099, 0x309A}, /* Japanese combining marks (dakuten/handakuten) */
    {0xA66F, 0xA672}, /* Cyrillic extended combining */
    {0xA674, 0xA67D},
    {0xA69E, 0xA69F},
    {0xFE20, 0xFE2F},   /* Combining Half Marks */
    {0x1F3FB, 0x1F3FF}, /* Emoji skin tone modifiers */
    {0xE0020, 0xE007F}, /* Emoji tag sequences */
};

/* Variation selectors: attach to the preceding code point but are
 * tracked separately from combining marks for clarity. */
static const struct codepoint_range GRAPHEME_CLUSTER_VARIATION_SELECTOR_RANGES[] = {
    {0xFE00, 0xFE0F},
    {0xE0100, 0xE01EF},
};

static bool codepoint_in_ranges(uint32_t codepoint, const struct codepoint_range *ranges, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (codepoint >= ranges[i].lo && codepoint <= ranges[i].hi) {
            return true;
        }
    }
    return false;
}

static bool grapheme_cluster_is_extend(uint32_t codepoint) {
    return codepoint_in_ranges(codepoint, GRAPHEME_CLUSTER_EXTEND_RANGES,
                               sizeof(GRAPHEME_CLUSTER_EXTEND_RANGES) / sizeof(GRAPHEME_CLUSTER_EXTEND_RANGES[0]));
}

static bool grapheme_cluster_is_variation_selector(uint32_t codepoint) {
    return codepoint_in_ranges(codepoint, GRAPHEME_CLUSTER_VARIATION_SELECTOR_RANGES,
                               sizeof(GRAPHEME_CLUSTER_VARIATION_SELECTOR_RANGES) / sizeof(GRAPHEME_CLUSTER_VARIATION_SELECTOR_RANGES[0]));
}

static bool grapheme_cluster_is_regional_indicator(uint32_t codepoint) {
    return codepoint >= 0x1F1E6u && codepoint <= 0x1F1FFu;
}

/* ------------------------------------------------------------------ */
/* Boundary logic                                                      */
/* ------------------------------------------------------------------ */

bool str_next_grapheme(const char **cursor, str_view *view) {
    const char *begin = *cursor;
    if (**cursor == '\0') {
        return false;
    }

    const char *p = begin;
    uint32_t codepoint = str_next_codepoint(&p);

    /* Rule: CR x LF (never split a Windows-style line ending) */
    if (codepoint == '\r') {
        const char *peek = p;
        uint32_t next = str_next_codepoint(&peek);
        if (next == '\n') {
            p = peek;
        }

        view->begin = begin;
        view->end = p;
        *cursor = p;
        return true;
    }

    /* Rule: Regional_Indicator x Regional_Indicator (flag emoji come in
     * pairs, and only ever pairs -- a run of 4 RIs is two flags, not
     * one cluster). */
    if (grapheme_cluster_is_regional_indicator(codepoint)) {
        const char *peek = p;
        uint32_t next = str_next_codepoint(&peek);
        if (grapheme_cluster_is_regional_indicator(next)) {
            p = peek;
        }

        view->begin = begin;
        view->end = p;
        *cursor = p;
        return true;
    }

    /* Extend the cluster with any trailing combining marks, variation
     * selectors, or ZWJ-joined code points. */
    for (;;) {
        const char *peek = p;
        uint32_t next = str_next_codepoint(&peek);

        if (next != 0 && (grapheme_cluster_is_extend(next) || grapheme_cluster_is_variation_selector(next))) {
            p = peek;
            continue;
        }

        if (next == GRAPHEME_CLUSTER_ZWJ) {
            /* ZWJ x <anything>: the joiner glues this cluster to
             * whatever code point follows it (used to build compound
             * emoji like multi-person families or professions). If the
             * ZWJ is dangling at the end of the string, stop before it
             * rather than consuming a code point that doesn't exist. */
            const char *after_zwj = peek;
            uint32_t joined = str_next_codepoint(&after_zwj);
            if (joined == 0) {
                break;
            }
            p = after_zwj;
            continue;
        }

        break;
    }

    view->begin = begin;
    view->end = p;
    *cursor = p;
    return true;
}

size_t str_graphemes(const str *s) {
    const char *p = s->data;
    str_view v;
    size_t count = 0;
    while (str_next_grapheme(&p, &v)) {
        count++;
    }
    return count;
}

str str_substr_grapheme_cluster(const str *s, size_t start_grapheme_cluster, size_t count_grapheme_cluster) {
    const char *p = s->data;
    str_view v;
    size_t idx = 0;

    while (idx < start_grapheme_cluster) {
        if (!str_next_grapheme(&p, &v)) {
            return str_create("");
        }
        idx++;
    }

    const char *byte_start_ptr = p;

    size_t taken = 0;
    while (taken < count_grapheme_cluster && str_next_grapheme(&p, &v)) {
        taken++;
    }

    size_t byte_start = (size_t)(byte_start_ptr - s->data);
    size_t byte_len = (size_t)(p - byte_start_ptr);

    return str_substr(s, byte_start, byte_len);
}