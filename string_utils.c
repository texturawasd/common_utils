#ifndef TEXTURAWASD_STRINGS
#define TEXTURAWASD_STRINGS

#include <stdlib.h>
#include <string.h>

void append(char **dst, size_t *used, const char *src)
{
    size_t n = strlen(src);
    memcpy(*dst + *used, src, n);
    *used += n;
}

void prepend(char **dst, size_t *used, const char *src)
{
    size_t n = strlen(src);
    memmove(*dst + n, *dst, *used);
    memcpy(*dst, src, n);
    *used += n;
}

#endif /* TEXTURAWASD_STRINGS */