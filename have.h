/* compat: for unix-like and windows */

#ifndef HAVE_H
#define HAVE_H
/* returns 1 if the command is found in PATH and executable, returns 0 if not. */
int command_exists(const char *command);

/* returns 1 if the command is found in PATH and executable, returns 0 if not, but fuzzy searches. */
int command_exists_fuzzy(const char *command);

/* Full path of the executable */
const char *which(const char *command);

#endif /* HAVE_H */