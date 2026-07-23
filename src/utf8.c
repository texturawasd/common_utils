#include "../utf8.h"
#include "../simple_strings.h"

/* ------------------------------------------------------------------ */
/* Decoding                                                           */
/* ------------------------------------------------------------------ */

uint32_t str_next_codepoint(const char **cursor) {
    const unsigned char *p = (const unsigned char *)*cursor;
    unsigned char b0 = p[0];

    if (b0 == 0) {
        return 0; /* end of string: don't advance */
    }

    if (b0 < 0x80) {
        *cursor += 1;
        return (uint32_t)b0;
    }

    int extra; /* number of continuation bytes expected */
    uint32_t codepoint;
    uint32_t min_codepoint; /* smallest code point that legitimately needs this many bytes */

    if ((b0 & 0xE0) == 0xC0) {
        extra = 1;
        codepoint = b0 & 0x1Fu;
        min_codepoint = 0x80;
    } else if ((b0 & 0xF0) == 0xE0) {
        extra = 2;
        codepoint = b0 & 0x0Fu;
        min_codepoint = 0x800;
    } else if ((b0 & 0xF8) == 0xF0) {
        extra = 3;
        codepoint = b0 & 0x07u;
        min_codepoint = 0x10000;
    } else {
        /* stray continuation byte, or a byte that can never start a
         * valid UTF-8 sequence (0x80-0xBF, 0xF8-0xFF) */
        *cursor += 1;
        return UTF8_REPLACEMENT_CHAR;
    }

    for (int i = 1; i <= extra; i++) {
        unsigned char bi = p[i];
        if (bi == 0 || (bi & 0xC0) != 0x80) {
            /* truncated sequence or bad continuation byte: consume only
             * the lead byte so we resync on the next call */
            *cursor += 1;
            return UTF8_REPLACEMENT_CHAR;
        }
        codepoint = (codepoint << 6) | (uint32_t)(bi & 0x3Fu);
    }

    /* Reject overlong encodings, surrogate halves, and out-of-range
     * code points. */
    if (codepoint < min_codepoint || codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu)) {
        *cursor += 1;
        return UTF8_REPLACEMENT_CHAR;
    }

    *cursor += (extra + 1);
    return codepoint;
}

uint32_t str_prev_codepoint(const char **cursor) {
    const unsigned char *start = (const unsigned char *)*cursor;
    const unsigned char *p = start;

    /* Walk backward over continuation bytes (10xxxxxx), at most 3 of
     * them, to find a plausible lead byte. */
    int stepped = 0;
    do {
        p--;
        stepped++;
    } while (stepped < 4 && (*p & 0xC0) == 0x80);

    const char *decode_ptr = (const char *)p;
    uint32_t codepoint = str_next_codepoint(&decode_ptr);

    if (decode_ptr != (const char *)start) {
        /* What we decoded forward from the lead byte we found doesn't
         * land back on where we started -- the bytes in between aren't
         * a single well-formed sequence. Treat just the one byte
         * immediately before `start` as a malformed unit. */
        *cursor = (const char *)(start - 1);
        return UTF8_REPLACEMENT_CHAR;
    }

    *cursor = (const char *)p;
    return codepoint;
}

/* ------------------------------------------------------------------ */
/* Encoding                                                           */
/* ------------------------------------------------------------------ */

static bool codepoint_is_valid_scalar(uint32_t codepoint) {
    return codepoint <= 0x10FFFFu && !(codepoint >= 0xD800u && codepoint <= 0xDFFFu);
}

int utf8_codepoint_len(uint32_t codepoint) {
    if (!codepoint_is_valid_scalar(codepoint)) {
        return 0;
    }
    if (codepoint < 0x80) {
        return 1;
    }
    if (codepoint < 0x800) {
        return 2;
    }
    if (codepoint < 0x10000) {
        return 3;
    }
    return 4;
}

int utf8_encode(uint32_t codepoint, char out[4]) {
    if (!codepoint_is_valid_scalar(codepoint)) {
        return 0;
    }

    if (codepoint < 0x80) {
        out[0] = (char)codepoint;
        return 1;
    }
    if (codepoint < 0x800) {
        out[0] = (char)(0xC0u | (codepoint >> 6));
        out[1] = (char)(0x80u | (codepoint & 0x3Fu));
        return 2;
    }
    if (codepoint < 0x10000) {
        out[0] = (char)(0xE0u | (codepoint >> 12));
        out[1] = (char)(0x80u | ((codepoint >> 6) & 0x3Fu));
        out[2] = (char)(0x80u | (codepoint & 0x3Fu));
        return 3;
    }
    out[0] = (char)(0xF0u | (codepoint >> 18));
    out[1] = (char)(0x80u | ((codepoint >> 12) & 0x3Fu));
    out[2] = (char)(0x80u | ((codepoint >> 6) & 0x3Fu));
    out[3] = (char)(0x80u | (codepoint & 0x3Fu));
    return 4;
}

/* ------------------------------------------------------------------ */
/* Code-point-aware String utilities                                   */
/* ------------------------------------------------------------------ */

size_t str_codepoints(const str *s) {
    const char *p = s->data;
    size_t count = 0;
    while (*p != '\0') {
        str_next_codepoint(&p);
        count++;
    }
    return count;
}

bool str_is_valid_utf8(const str *s) {
    const unsigned char *p = (const unsigned char *)s->data;

    while (*p != '\0') {
        unsigned char b0 = *p;

        if (b0 < 0x80) {
            p += 1;
            continue;
        }

        int extra;
        uint32_t codepoint;
        uint32_t min_codepoint;

        if ((b0 & 0xE0) == 0xC0) {
            extra = 1;
            codepoint = b0 & 0x1Fu;
            min_codepoint = 0x80;
        } else if ((b0 & 0xF0) == 0xE0) {
            extra = 2;
            codepoint = b0 & 0x0Fu;
            min_codepoint = 0x800;
        } else if ((b0 & 0xF8) == 0xF0) {
            extra = 3;
            codepoint = b0 & 0x07u;
            min_codepoint = 0x10000;
        } else {
            return false;
        }

        for (int i = 1; i <= extra; i++) {
            unsigned char bi = p[i];
            if (bi == 0 || (bi & 0xC0) != 0x80) {
                return false;
            }
            codepoint = (codepoint << 6) | (uint32_t)(bi & 0x3Fu);
        }

        if (codepoint < min_codepoint || codepoint > 0x10FFFFu || (codepoint >= 0xD800u && codepoint <= 0xDFFFu)) {
            return false;
        }

        p += (extra + 1);
    }

    return true;
}

str str_substr_codepoint(const str *s, size_t start_codepoint, size_t count_codepoint) {
    const char *p = s->data;
    size_t idx = 0;

    /* advance to the start code point, tracking byte offset */
    while (idx < start_codepoint && *p != '\0') {
        str_next_codepoint(&p);
        idx++;
    }

    if (*p == '\0') {
        return str_create("");
    }

    size_t byte_start = (size_t)(p - s->data);

    size_t taken = 0;
    while (taken < count_codepoint && *p != '\0') {
        str_next_codepoint(&p);
        taken++;
    }

    size_t byte_len = (size_t)(p - s->data) - byte_start;

    return str_substr(s, byte_start, byte_len);
}