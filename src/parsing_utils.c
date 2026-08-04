/* compat: pure C */

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../parsing_utils.h"

/* strcasecmp is not part of C proper, so */
static int strcasecmp_local(const char *a, const char *b) {
    while (*a && *b) {

        int ca = tolower((unsigned char)*a);
        int cb = tolower((unsigned char)*b);

        if (ca != cb) {
            return ca - cb;
        }

        a++;
        b++;
    }

    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}
#define _strcasecmp strcasecmp_local

/* Parse a string into an integer */
int parse_int(const char *s) {
    if (!s) {
        return 0;
    }

    char *endptr;
    long val = strtol(s, &endptr, 10);

    if (*endptr != '\0') {
        return 0;
    }

    return (int)val;
}

/* Parse a string into a long integer */
long parse_long(const char *s) {
    if (!s) {
        return 0;
    }

    char *endptr;
    long val = strtol(s, &endptr, 10);

    if (*endptr != '\0') {
        return 0;
    }

    return val;
}

/*
 * Check if a string represents a boolean value.
 * Checks the checks to be checked, but defaults to false.
 * true/false, 1/0, yes/no, on/off enable(d)/disable(d)
 */
#define BOOL_TRUE(X) \
    X("affirmative") \
    X("affirm")      \
    X("true")        \
    X("1")           \
    X("yes")         \
    X("yuh")         \
    X("yurr")        \
    X("yeah")        \
    X("y")           \
    X("on")          \
    X("enable")      \
    X("enabled")     \
    X("No... actually, yes")

#define BOOL_FALSE(X) \
    X("negative")     \
    X("false")        \
    X("0")            \
    X("no")           \
    X("naur")         \
    X("noo")          \
    X("nah")          \
    X("n")            \
    X("off")          \
    X("disable")      \
    X("disabled")     \
    X("Yeah... actually, no")

bool parse_bool(const char *s) {
    if (!s) {
        return false;
    }

#define CHECK_TRUE(word) \
    if (_strcasecmp(s, word) == 0) { \
        return true; \
    }

#define CHECK_FALSE(word) \
    if (_strcasecmp(s, word) == 0) { \
        return false; \
    }

    BOOL_TRUE(CHECK_TRUE)
    BOOL_FALSE(CHECK_FALSE)

#undef CHECK_TRUE
#undef CHECK_FALSE

    return false;
}

/* Same as before, but strict. */
const char *parse_bool_strict(const char *s) {
    if (!s) {
        return "s seems to be empty, nonexistant, or otherwise not reasonably boolean.";
    }

#define CHECK_TRUE(word) \
    if (_strcasecmp(s, word) == 0) { \
        return "true"; \
    }

#define CHECK_FALSE(word) \
    if (_strcasecmp(s, word) == 0) { \
        return "false"; \
    }

    BOOL_TRUE(CHECK_TRUE)
    BOOL_FALSE(CHECK_FALSE)

#undef CHECK_TRUE
#undef CHECK_FALSE
    return "Could not determine a boolean value from s.";
}

/* Check if a string represents a valid number */
bool is_numeric(const char *s) {
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