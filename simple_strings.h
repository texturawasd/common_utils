#ifndef _SIMPLE_STRINGS_H
#define _SIMPLE_STRINGS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/*
 * Strings, but they feel natural, but they're a string.
 * rule of thumb: `String str; str.data` is the same as `char *str; str` in normal C.
 * that's the idiomatic way to use it as a normal C string.
 */

typedef struct String {
    char *data; /* String contents */
    size_t len; /* Bytes, excluding '\0' */
    size_t cap; /* Allocated bytes */
} string;

#define str string

/*
 * Constructors and Memory Management
 */

/* Create a new String from a C string. */
str str_create(const char *src);

/* Create a new String with initial capacity. */
str str_with_cap(size_t cap);

/* Free the String's allocated memory. */
void str_destroy(str *s);

/* Convert String to const char * (explicit conversion). */
const char *cstr(const str *s);

/*
 * String Modification
 */

/* Append a C string to the String. Returns String* for chaining. */
str *str_append(str *s, const char *suffix);

/* Append a String to a String. Returns String. */
str str_append_str(str *s, str *suffix);

/* Prepend a C string to the String. Returns String* for chaining. */
str *str_prepend(str *s, const char *prefix);

/* Trim leading and trailing whitespace. Returns String*. */
str *str_trim(str *s);

/* Trim leading whitespace. Returns String*. */
str *str_ltrim(str *s);

/* Trim trailing whitespace. Returns String*. */
str *str_rtrim(str *s);

/* Remove one trailing line ending (\n, \r, or \r\n). Returns String*. */
str *str_chomp(str *s);

/* Remove all trailing line endings. Returns String*. */
str *str_chomp_all(str *s);

/* Chop off a character from s. */
str *str_chop(str *s);

/* Chop off multiple characters from s. */
str *str_chop_n(str *s, size_t n);

/* Convert string to uppercase. Returns String*. */
str *to_upper(str *s);

/* Convert string to lowercase. Returns String*. */
str *to_lower(str *s);

/*
 * String Queries
 */

/* Check if string ends with suffix. */
bool str_ends_with(const str *s, const char *suffix);

/* Check if string starts with prefix. */
bool str_starts_with(const str *s, const char *prefix);

/* Find the first occurrence of substring. Returns pointer or NULL. */
const char *str_find(const str *s, const char *substr);

/* Get substring in quotes. Extracts text between `"..."`. */
str str_quoted_substring(const char *src);

/*
 * Utility Functions
 */

/* Join an array of C strings with separator. */
str str_join(const char *sep, const char **strs, size_t count);

/* Join an array of Strings with separator */
str str_join_strs(const char *sep, const char strings[], size_t count);

/* Repeat a string n times. */
str str_repeat(const char *src, int n);

/* Replace all occurrences of old with new. */
str str_replace(const char *src, const char *old, const char *new_str);

/* Remove prefix if present. */
str str_remove_prefix(const char *src, const char *prefix);

/* Remove suffix if present. */
str str_remove_suffix(const char *src, const char *suffix);

/* Check if two strings are equal. */
bool str_eq(const str *a, const str *b);

/* Compare two strings. Returns <0, 0, or >0 like strcmp. */
int str_cmp(const str *a, const str *b);

/* Clear the string contents without freeing memory. */
void str_clear(str *s);

/* Reserve capacity for the string. */
void str_reserve(str *s, size_t cap);

/* Extract a substring. */
str str_substr(const str *s, size_t start, size_t len);

/* Insert a substring at position. Returns str* for chaining. */
str *str_insert(str *s, size_t pos, const char *substr);

/* Erase characters from position. Returns str* for chaining. */
str *str_erase(str *s, size_t pos, size_t len);

/* Clone a string. */
str str_clone(const str *s);

/* Split a string by delimiter. Returns array of strings, count in out_count. */
str *str_split(const str *s, const char *delim, size_t *out_count);

#define to_cstr(s) cstr(s) /* alias for cstr() */
#define NULL_STRING str_create("")

/*
 * Unicode
 *
 *     String (bytes)
 *         v
 *     UTF-8 decoding/encoding
 *         v
 *     Unicode code points            <-- always available, src/utf8.h
 *         v
 *     Unicode grapheme clusters      <-- optional, src/grapheme.h
 *
 * Everything in this section is opt-in in the sense that it costs
 * nothing if you never call it: none of the existing byte-based API
 * above changes behavior or gains overhead. `str str; str.data` used as
 * a plain C string still works exactly as before.
 */

/* Always available: code point decoding and code-point-aware queries. */
#include "utf8.h"

/* Optional: user-perceived character (grapheme cluster) support. Define
 * SIMPLE_STRINGS_GRAPHEME_CLUSTER_SUPPORT before including this header
 * if you need it; otherwise str_graphemes()/str_substr_gc()/
 * str_next_grapheme() and their implementation are compiled out
 * entirely. */
#ifdef SIMPLE_STRINGS_GRAPHEME_CLUSTER_SUPPORT
#include "grapheme.h"
#endif

#endif /* _SIMPLE_STRINGS_H */

/* Implementation */
#ifdef SIMPLE_STRINGS_IMPLEMENTATION
#include "src/strings.c"
#include "src/utf8.c"
#ifdef SIMPLE_STRINGS_GRAPHEME_CLUSTER_SUPPORT
#include "src/grapheme.c"
#endif
#endif