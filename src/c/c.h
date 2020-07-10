#include <string.h>
#include <stdlib.h>

#ifndef HAMBURGER_HELPER_H
#define HAMBURGER_HELPER_H

#include "c/files.h"

typedef int uns_t;

#define ERROR_TXT "error.txt"

static void log_(const char *p)
{
    printf("%s\n", p);
}

#define cassert_(e, m)       \
    if (!(e))               \
    {                       \
        fout(ERROR_TXT, m); \
        exit(1);            \
    }

#endif