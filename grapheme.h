#ifndef _SIMPLE_STRINGS_GRAPHEME_H
#define _SIMPLE_STRINGS_GRAPHEME_H

#include "simple_strings.h"

/*
 * Grapheme cluster layer: code points -> user-perceived characters.
 *
 * A "grapheme cluster" is what a person would call a single character:
 * "e" + combining acute accent is one grapheme cluster made of two code
 * points; a flag emoji is one grapheme cluster made of two Regional
 * Indicator code points; "family: man, woman, girl, boy" joined with
 * ZWJ is one grapheme cluster made of many code points.
 *
 * IMPORTANT: this is a practical, simplified implementation, not a full
 * conformant implementation of Unicode Annex #29 (UAX #29). It correctly
 * handles the cases that come up constantly in real text and real emoji:
 *
 *   - CRLF as a single cluster
 *   - combining marks (accents, diacritics, vowel signs, tone marks)
 *     attaching to the base character before them
 *   - variation selectors (e.g. U+FE0F) attaching to the preceding
 *     code point
 *   - ZWJ-joined sequences (emoji like a family or a profession emoji
 *     with a skin tone modifier)
 *   - Regional Indicator pairs (flag emoji)
 *
 * It does NOT implement every rule in UAX #29 (e.g. some Indic script
 * conjunct/viramaconsonant edge cases, Hangul jamo composition rules,
 * prepended concatenation marks, or prose_boundary/extended pictographic
 * lookahead beyond what's listed above). If you need strict, fully
 * conformant UAX #29 segmentation, reach for a dedicated Unicode library
 * such as utf8proc or ICU instead. For most applications (search,
 * cursor movement, "how many characters is this", truncating strings
 * without splitting an emoji or an accent off its letter) this covers
 * the practical cases.
 */

#include "utf8.h"

/* Like utf8.h, this header is always pulled in from simple_strings.h
 * after the `str` type is already defined, so it doesn't include
 * simple_strings.h itself. Don't include grapheme.h on its own; include
 * simple_strings.h (with SIMPLE_STRINGS_GRAPHEME_CLUSTER_SUPPORT
 * defined) instead. */

/* Count the number of grapheme clusters (user-perceived characters) in s. */
size_t str_graphemes(const str *s);

/*
 * Extract a substring by grapheme cluster offsets rather than byte or
 * code point offsets. start_grapheme_cluster is the index of the first cluster to
 * include; count_grapheme_cluster is how many clusters to include (clamped to the end
 * of the string). If start_grapheme_cluster is past the end, returns an empty string.
 */
str str_substr_grapheme_cluster(const str *s, size_t start_grapheme_cluster, size_t count_grapheme_cluster);

/*
 * Advance *cursor past one grapheme cluster and report its byte range in
 * *view (view->begin/view->end point into the same buffer as *cursor,
 * no allocation happens). Returns false (and leaves *cursor unchanged)
 * once *cursor is at the end of the string.
 *
 * Typical use:
 *
 *     const char *p = cstr(&s);
 *     str_view v;
 *     while (str_next_grapheme(&p, &v)) {
 *         printf("cluster: %.*s\n", (int)(v.end - v.begin), v.begin);
 *     }
 */
bool str_next_grapheme(const char **cursor, str_view *view);

#endif /* _SIMPLE_STRINGS_GRAPHEME_H */