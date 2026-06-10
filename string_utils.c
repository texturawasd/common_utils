#ifndef TEXTURAWASD_STRINGS
#define TEXTURAWASD_STRINGS

/* June 2026 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/*
 * Strings, but they feel natural, but they're a string.
 * rule of thumb: `String str; str.data` is the same as `char *str; str` in normal C.
 * that's the idiomatic way to use it as a normal C string.
 */


typedef struct String {
    char *data;
    size_t len;
    size_t cap;
} string;

#define str string

/*
 * Constructors and Memory Management
 */

/* Create a new String from a C string. */
str str_create(const char *src)
{
    str s = {0};
    if (src) {
        size_t len = strlen(src);
        s.cap = len + 1;
        s.data = malloc(s.cap);
        if (s.data) {
            memcpy(s.data, src, len);
            s.data[len] = '\0';
            s.len = len;
        }
    } else {
        s.cap = 1;
        s.data = malloc(1);
        if (s.data) {
            s.data[0] = '\0';
            s.len = 0;
        }
    }
    return s;
}

/* Create a new String with initial capacity. */
str str_with_cap(size_t cap)
{
    str s = {0};
    s.cap = cap > 0 ? cap : 1;
    s.data = malloc(s.cap);
    if (s.data) {
        s.data[0] = '\0';
        s.len = 0;
    }
    return s;
}

/* Free the String's allocated memory. */
void str_destroy(str *s)
{
    if (s && s->data) {
        free(s->data);
        s->data = NULL;
        s->len = 0;
        s->cap = 0;
    }
}

/* Convert String to const char * (explicit conversion). */
const char *cstr(const str *s)
{
    return (s && s->data) ? s->data : "";
}

#define to_cstr(s) cstr(s) /* alias for cstr() */


/*
 * Growing and Capacity
 */

/* Grow the String's capacity. */
static void str_grow(str *s, size_t additional)
{
    if (!s || !s->data) return;

    size_t needed = s->len + additional + 1;
    if (needed <= s->cap) return;

    size_t new_cap = s->cap * 2;
    while (new_cap < needed) {
        new_cap *= 2;
    }

    char *new_data = realloc(s->data, new_cap);
    if (new_data) {
        s->data = new_data;
        s->cap = new_cap;
    }
}


/*
 * String Modification
*/

/* Append a C string to the String. Returns String* for chaining. */
str *str_append(str *s, const char *suffix)
{
    if (!s || !s->data || !suffix) {
        return s;
    }

    size_t n = strlen(suffix);
    if (n == 0) {
        return s;
    }

    str_grow(s, n);
    memcpy(s->data + s->len, suffix, n);
    s->len += n;
    s->data[s->len] = '\0';

    return s;
}

/* Prepend a C string to the String. Returns String* for chaining. */
str *str_prepend(str *s, const char *prefix)
{
    if (!s || !s->data || !prefix) {
        return s;
    }

    size_t n = strlen(prefix);
    if (n == 0) {
        return s;
    }

    str_grow(s, n);
    memmove(s->data + n, s->data, s->len + 1);
    memcpy(s->data, prefix, n);
    s->len += n;

    return s;
}

/* Trim leading and trailing whitespace. Returns String*. */
str *str_trim(str *s)
{
    if (!s || !s->data) {
        return s;
    }

    /* Trim leading */
    size_t start = 0;
    while (start < s->len && isspace((unsigned char)s->data[start])) {
        start++;
    }

    /* Trim trailing */
    size_t end = s->len;
    while (end > start && isspace((unsigned char)s->data[end - 1])) {
        end--;
    }

    if (start > 0) {
        memmove(s->data, s->data + start, end - start);
    }
    s->len = end - start;
    s->data[s->len] = '\0';

    return s;
}

/* Trim leading whitespace. Returns String*. */
str *str_ltrim(str *s)
{
    if (!s || !s->data) {
        return s;
    }

    size_t start = 0;
    while (start < s->len && isspace((unsigned char)s->data[start])) {
        start++;
    }

    if (start > 0) {
        memmove(s->data, s->data + start, s->len - start);
        s->len -= start;
        s->data[s->len] = '\0';
    }

    return s;
}

/* Trim trailing whitespace. Returns String*. */
str *str_rtrim(str *s)
{
    if (!s || !s->data) {
        return s;
    }

    while (s->len > 0 && isspace((unsigned char)s->data[s->len - 1])) {
        s->len--;
    }
    s->data[s->len] = '\0';

    return s;
}

/* Convert string to uppercase. Returns String*. */
str *to_upper(str *s)
{
    if (!s || !s->data) {
        return s;
    }

    for (size_t i = 0; i < s->len; i++) {
        s->data[i] = toupper((unsigned char)s->data[i]);
    }

    return s;
}

/* Convert string to lowercase. Returns String*. */
str *to_lower(str *s)
{
    if (!s || !s->data) {
        return s;
    }

    for (size_t i = 0; i < s->len; i++) {
        s->data[i] = tolower((unsigned char)s->data[i]);
    }

    return s;
}


/*
 * String Queries
 */

/* Check if string ends with suffix. */
bool str_ends_with(const str *s, const char *suffix)
{
    if (!s || !suffix) {
        return false;
    }

    size_t suffix_len = strlen(suffix);
    if (suffix_len > s->len) {
        return false;
    }

    return memcmp(s->data + s->len - suffix_len, suffix, suffix_len) == 0;
}

/* Check if string starts with prefix. */
bool str_starts_with(const str *s, const char *prefix)
{
    if (!s || !prefix) {
        return false;
    }

    size_t prefix_len = strlen(prefix);
    if (prefix_len > s->len) {
        return false;
    }

    return memcmp(s->data, prefix, prefix_len) == 0;
}

/* Find the first occurrence of substring. Returns pointer or NULL. */
const char *str_find(const str *s, const char *substr)
{
    if (!s || !substr) {
        return NULL;
    }

    return strstr(s->data, substr);
}

/* Get substring in quotes. Extracts text between `"..."`. */
str str_quoted_substring(const char *src)
{
    str s = {0};
    if (!src) {
        return s;
    }

    const char *start = strchr(src, '"');
    if (!start) {
        return s;
    }

    start++;
    const char *end = strchr(start, '"');
    if (!end) {
        return s;
    }

    size_t len = end - start;
    if (s.data) {
        free(s.data);
    }
    s.cap = len + 1;
    s.data = malloc(s.cap);
    if (s.data) {
        memcpy(s.data, start, len);
        s.data[len] = '\0';
        s.len = len;
    }

    return s;
}


/*
 * Utility Functions
 */

/* Join an array of C strings with separator. */
str str_join(const char *sep, const char **strs, size_t count)
{
    str s = str_create("");
    if (!sep || !strs) {
        return s;
    }

    for (size_t i = 0; i < count; i++) {
        if (i > 0) {
            str_append(&s, sep);
        }
        str_append(&s, strs[i]);
    }

    return s;
}

/* Repeat a string n times. */
str str_repeat(const char *src, int n)
{
    str s = str_create("");
    if (!src || n <= 0) {
        return s;
    }

    for (int i = 0; i < n; i++) {
        str_append(&s, src);
    }

    return s;
}

/* Replace all occurrences of old with new. */
str str_replace(const char *src, const char *old, const char *new_str)
{
    str s = str_create("");
    if (!src || !old || !new_str) {
        return str_create(src);
    }

    size_t old_len = strlen(old);
    if (old_len == 0) {
        return str_create(src);
    }

    const char *ptr = src;
    const char *match;

    while ((match = strstr(ptr, old))) {
        size_t len = match - ptr;

        if (len > 0) {
            char *tmp = malloc(len + 1);
            if (tmp) {
                memcpy(tmp, ptr, len);
                tmp[len] = '\0';
                str_append(&s, tmp);
                free(tmp);
            }
        }

        str_append(&s, new_str);
        ptr = match + old_len;
    }

    str_append(&s, ptr);  /* append remainder */
    return s;
}

/* Remove prefix if present. */
str str_remove_prefix(const char *src, const char *prefix)
{
    if (!src || !prefix) {
        return str_create(src);
    }

    size_t prefix_len = strlen(prefix);
    size_t src_len = strlen(src);

    if (src_len >= prefix_len && memcmp(src, prefix, prefix_len) == 0) {
        return str_create(src + prefix_len);
    }

    return str_create(src);
}

/* Remove suffix if present. */
str str_remove_suffix(const char *src, const char *suffix)
{
    if (!src || !suffix) {
        return str_create(src);
    }

    size_t suffix_len = strlen(suffix);
    size_t src_len = strlen(src);

    if (src_len >= suffix_len &&
        memcmp(src + src_len - suffix_len, suffix, suffix_len) == 0) {
        str s = str_create(src);
        s.len = src_len - suffix_len;
        s.data[s.len] = '\0';
        return s;
    }

    return str_create(src);
}
#endif /* TEXTURAWASD_STRINGS */