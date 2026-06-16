/* compat: for unix-like systems only */

#ifndef HAVE_H
#define HAVE_H

#include "src/have.c"

int command_exists(const char *command);

const char *which(const char *command);

#endif /* HAVE_H */