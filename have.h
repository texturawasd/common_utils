/* compat: for unix-like systems only */

#ifndef HAVE_H
#define HAVE_H

int command_exists(const char *command);

const char *which(const char *command);

#endif /* HAVE_H */

/* Implementation */
#ifdef HAVE_IMPLEMENTATION
#include "src/have.c"
#endif