#ifndef _SIMPLE_STRINGS_H
#define _SIMPLE_STRINGS_H

#include <stdlib.h>
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

/* Forward declarations */
str str_create(const char *src);
str str_with_cap(size_t cap);
void str_destroy(str *s);
const char *cstr(const str *s);
str *str_append(str *s, const char *suffix);
str *str_prepend(str *s, const char *prefix);
str *str_trim(str *s);
str *str_ltrim(str *s);
str *str_rtrim(str *s);
str *to_upper(str *s);
str *to_lower(str *s);
bool str_ends_with(const str *s, const char *suffix);
bool str_starts_with(const str *s, const char *prefix);
const char *str_find(const str *s, const char *substr);
str str_quoted_substring(const char *src);
str str_join(const char *sep, const char **strs, size_t count);
str str_repeat(const char *src, int n);

#define NULL_STRING str_create("")


#endif /* _SIMPLE_STRINGS_H */

/* implementation */
#ifndef _SIMPLE_STRINGS_IMPL
#include "src/string_utils.c"
#endif