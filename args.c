/* compat: pure C */

/* have you seen arg.h [https://git.suckless.org/dmenu/file/arg.h.html]? i wanted something for my lowly mortal brain instead. */

#ifndef TEXTURAWASD_ARG
#define TEXTURAWASD_ARG


#include <stdbool.h>
#include "string_utils.c"

bool arg_is_present(const char *arg, int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], arg) == 0) {
            return true;
        }
    }
    return false;
}

bool arg_is_present_with_value(const char *arg, const char *value, int argc, char **argv);
#error unimplemented

bool arg_is_present_at_nth_position(const char *arg, int n);
#error unimplemented

bool arg_is_in_list(const char *arg, const char *list);
#error unimplemented

#endif