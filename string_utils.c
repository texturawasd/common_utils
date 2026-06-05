#ifndef TEXTURAWASD_STRINGS
#define TEXTURAWASD_STRINGS

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void append(char **dst, size_t *used, const char *src)
{
    if (!dst || !*dst || !used || !src)
        return;
    size_t n = strlen(src);
    memcpy(*dst + *used, src, n);
    *used += n;
    /* ensure destination is NUL-terminated */
    (*dst)[*used] = '\0';
}

void prepend(char **dst, size_t *used, const char *src)
{
    if (!dst || !*dst || !used || !src)
        return;
    size_t n = strlen(src);
    memmove(*dst + n, *dst, *used);
    memcpy(*dst, src, n);
    *used += n;
    /* ensure destination is NUL-terminated */
    (*dst)[*used] = '\0';
}

void trim(char **str)
{
    if (!str || !*str)
        return;
    char *s = *str;
    while (isspace((unsigned char)*s))
        s++;
    if (*s == 0) {
        *str = s;
        return;
    }
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
    *str = s;
}

void ltrim(char **str)
{
    if (!str || !*str)
        return;
    char *s = *str;
    while (isspace((unsigned char)*s))
        s++;
    *str = s;
}

void rtrim(char **str)
{
    if (!str || !*str)
        return;
    char *s = *str;
    size_t len = strlen(s);
    if (len == 0)
        return;
    char *end = s + len - 1;
    while (end > s && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
}

void join_strings(char **dst, size_t *used, const char *sep, const char **strs, size_t count)
{
    if (!dst || !*dst || !used || !strs)
        return;
    for (size_t i = 0; i < count; i++) {
        if (i > 0)
            append(dst, used, sep);
        append(dst, used, strs[i]);
    }
}
#endif /* TEXTURAWASD_STRINGS */