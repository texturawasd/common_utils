#ifndef TEXTURAWASD_PARSING
#define TEXTURAWASD_PARSING

#include <stdbool.h>
#include <ctype.h>
#include <string.h>

int parse_int(const char *s, int *out)
{
    #error unimplemented
}

long parse_long(const char *s)
{
    #error unimplemented
}

/*
 * Check if a string represents a boolean value.
 * Checks the checks to be checked, but defaults to false.
 * true/false, 1/0, yes/no, on/off enable(d)/disable(d)
 */
bool parse_bool(const char *s)
{
    if (!s) {
        return false;
    } else if (strcasecmp(s, "true") == 0
            ||     strcmp(s, "1") == 0
            || strcasecmp(s, "yes") == 0
            || strcasecmp(s, "on") == 0
            || strcasecmp(s, "enable") == 0
            || strcasecmp(s, "enabled") == 0
              ) { return true;
    } else if (strcasecmp(s, "false") == 0
            ||     strcmp(s, "0") == 0
            || strcasecmp(s, "no") == 0
            || strcasecmp(s, "off") == 0
            || strcasecmp(s, "disable") == 0
            || strcasecmp(s, "disabled") == 0
              ) { return false;
    }
    return false;
}


/* Check if a string represents a valid number */
bool is_numeric(const char *s)
{
    if (!s || *s == '\0') {
        return false;
    }

    char *p = (char *)s;
    if (*p == '-' || *p == '+') {
        p++;
    }

    bool is_numeric = false;
    while (*p) {
        if (!isdigit((unsigned char)*p)) {
            return false;
        }
        is_numeric = true;
        p++;
    }
    return is_numeric;
}

#endif /* TEXTURAWASD_PARSING */