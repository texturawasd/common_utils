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

/* gets the "substring" in quotes (`""`) from the input string. example:
 input is `dasda"fls"ghj`, then output is `fls`*/
char *get_quoted_substring(const char *str)
{
    if (!str)
        return NULL;

    const char *start = strchr(str, '"');
    if (!start)
        return NULL;

    start++; // move past opening quote

    const char *end = strchr(start, '"');
    if (!end)
        return NULL;

    size_t len = end - start;

    char *result = malloc(len + 1);
    if (!result)
        return NULL;

    memcpy(result, start, len);
    result[len] = '\0';

    return result;
}
#endif /* TEXTURAWASD_STRINGS */