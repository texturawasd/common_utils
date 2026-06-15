/* compat: pure C */

/* abstraction for arguments */

#ifndef TEXTURAWASD_ARG
#define TEXTURAWASD_ARG

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Internal helpers */

/*
 * Strip any leading prefix characters: `-`, `--`, `/`. (yes, Windows-style too)
 * Returns a pointer into the original string past the prefix.
 */
static const char *strip_prefix(const char *arg)
{
    if (arg[0] == '-' && arg[1] == '-') return arg + 2;
    if (arg[0] == '-' || arg[0] == '/') return arg + 1;
    return arg;
}

/*
 * Given a raw argv token like "--dir=/var/www/files" or "-v",
 * extract its key (the part before '=') into key_out (caller supplies buffer).
 * Returns a pointer to the value (part after '='), or NULL if no '=' present.
 */
static const char *split_key_value(const char *raw, char *key_out, size_t key_out_size)
{
    const char *stripped = strip_prefix(raw);
    const char *eq = strchr(stripped, '=');

    if (eq == NULL) {
        snprintf(key_out, key_out_size, "%s", stripped);
        return NULL;
    }

    size_t key_len = (size_t)(eq - stripped);
    if (key_len >= key_out_size) key_len = key_out_size - 1;
    memcpy(key_out, stripped, key_len);
    key_out[key_len] = '\0';

    return eq + 1;
}

/* API */

/*
 * Returns true if the named argument is present in any form:
 *   -verbose  --verbose  /verbose
 * Position-independent. */
bool arg_is_present(const char *arg, int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        char key[256];
        split_key_value(argv[i], key, sizeof(key));
        if (strcmp(key, arg) == 0) return true;
    }
    return false;
}

/*
 * Returns true if the named argument is present AND has the given value,
 * in any of these forms:
 *   --dir=/var/www/files   -dir=/var/www/files   /dir=/var/www/files
 */
bool arg_is_present_with_value(const char *arg, const char *value, int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        char key[256];
        const char *val = split_key_value(argv[i], key, sizeof(key));
        if (strcmp(key, arg) == 0 && val != NULL && strcmp(val, value) == 0)
            return true;
    }
    return false;
}

/*
 * Returns true if the named argument appears at the nth position (1-based).
 * Prefixes are stripped before comparison, so --verbose, -verbose, /verbose
 * all match "verbose" at position n.
 */
bool arg_is_present_at_nth_position(const char *arg, int n, int argc, char **argv)
{
    if (n < 1 || n >= argc) return false;
    char key[256];
    split_key_value(argv[n], key, sizeof(key));
    return strcmp(key, arg) == 0;
}

/*
 * Returns true if arg is found in a comma-separated list string.
 * e.g. arg_is_in_list("verbose", "help,verbose,debug") -> true
 */
bool arg_is_in_list(const char *arg, const char *list)
{
    if (!arg || !list) return false;

    char buf[1024];
    snprintf(buf, sizeof(buf), "%s", list);

    char *token = strtok(buf, ",");
    while (token != NULL) {
        if (strcmp(token, arg) == 0) return true;
        token = strtok(NULL, ",");
    }
    return false;
}

/*
 * Returns the value associated with the named key, or NULL if not found /
 * no value present. The returned pointer points into the original argv
 * string - do not free it.
 *   --dir=/var/www/files  =>  get_arg_value("dir", ...) returns "/var/www/files"
 */
const char *get_arg_value(const char *arg, int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        char key[256];
        const char *val = split_key_value(argv[i], key, sizeof(key));
        if (strcmp(key, arg) == 0) return val;
    }
    return NULL;
}

/*
 * Returns true if the named key has the given value, regardless of prefix.
 *   --dir=/var/www/files  ->  config_arg_is_val("dir", "/var/www/files", ...) -> true
 */
bool config_arg_is_val(const char *key, const char *val, int argc, char **argv)
{
    const char *found = get_arg_value(key, argc, argv);
    if (found == NULL) return false;
    return strcmp(found, val) == 0;
}

#endif /* TEXTURAWASD_ARGS */