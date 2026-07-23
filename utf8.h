#ifndef _SIMPLE_STRINGS_UTF8_H
#define _SIMPLE_STRINGS_UTF8_H

/*
 * UTF-8 layer: bytes <-> code points.
 *
 * This layer knows nothing about "characters" as a human would count them
 * (that's the grapheme layer, see grapheme.h). It only knows how to walk
 * a byte buffer and decode/encode Unicode code points per the UTF-8 spec.
 *
 *     String (bytes)
 *         v
 *     UTF-8 decoding/encoding      <-- this file
 *         v
 *     Unicode code points
 *         v
 *     Unicode grapheme clusters (optional, grapheme.h)
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* This header is always pulled in from simple_strings.h, after the `str`
 * type is already defined, so it doesn't include simple_strings.h itself
 * (doing so would re-enter that file's implementation-include section at
 * the bottom and recurse). Don't include utf8.h on its own; include
 * simple_strings.h instead. */

/* The Unicode replacement character, substituted for any malformed byte
 * sequence encountered while decoding. */
#define UTF8_REPLACEMENT_CHAR 0xFFFDu

/*
 * A non-owning view into a byte range: [begin, end). Not necessarily
 * NUL-terminated. Used to hand back slices (e.g. single grapheme clusters)
 * without allocating.
 */
typedef struct String_View {
    const char *begin;
    const char *end;
} string_view;
#define str_view string_view

/*
 * Decode the code point starting at *cursor and advance *cursor past it.
 *
 *  - If *cursor points at a NUL byte, returns 0 and does NOT advance
 *    (mirrors str_next_grapheme's "end of string" convention: check the
 *    byte at *cursor yourself, or just watch for a 0 return with no
 *    forward progress, before calling again).
 *  - If the bytes at *cursor are not a valid UTF-8 sequence (bad
 *    continuation bytes, overlong encoding, surrogate half, or a code
 *    point above U+10FFFF), returns UTF8_REPLACEMENT_CHAR and advances by
 *    exactly 1 byte, so callers always make forward progress on garbage
 *    input instead of looping forever.
 */
uint32_t str_next_codepoint(const char **cursor);

/*
 * Step *cursor back by exactly one code point and return the code point
 * that was stepped over. Same replacement-character behavior as
 * str_next_codepoint on malformed input.
 *
 * Caller's responsibility: *cursor must not be stepped before the start
 * of the buffer it came from. This function only looks backward through
 * memory that is known to belong to that buffer.
 */
uint32_t str_prev_codepoint(const char **cursor);

/* Number of bytes needed to encode `codepoint` in UTF-8 (1-4), or 0 if `codepoint` is
 * not a valid Unicode scalar value (i.e. a surrogate half or > U+10FFFF). */
int utf8_codepoint_len(uint32_t codepoint);

/* Encode `codepoint` into `out`, which must have room for at least 4 bytes.
 * Returns the number of bytes written (1-4), or 0 (writes nothing) if
 * `codepoint` is not a valid Unicode scalar value. */
int utf8_encode(uint32_t codepoint, char out[4]);

#endif /* _SIMPLE_STRINGS_UTF8_H */