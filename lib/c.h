#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef HAMBURGER_HELPER_H
#define HAMBURGER_HELPER_H

#include "files.h"

typedef int uns_t;

#define ERROR_TXT "error.txt"

static void log_(const char *p)
{
    printf("%s\n", p);
}

#define cassert_(e, m)       \
    if (!(e))                \
    {                        \
        printf(m);           \
        cfout(ERROR_TXT, m); \
        exit(1);             \
    }

#endif