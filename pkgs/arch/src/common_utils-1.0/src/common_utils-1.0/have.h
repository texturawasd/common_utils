/* compat: for unix-like and windows */

#ifndef HAVE_H
#define HAVE_H

int command_exists(const char *command);

const char *which(const char *command);

#endif /* HAVE_H */
