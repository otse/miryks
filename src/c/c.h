#include <stdlib.h>

#ifndef HAMBURGER_HELPER_H
#define HAMBURGER_HELPER_H

#include "c/files.h"

#define ERROR_TXT "error.txt"

static void log_(const char *p)
{
    printf("%s\n", p);
}

#define assert_(e, m)       \
    if (!(e))               \
    {                       \
        fout(ERROR_TXT, m); \
        exit(1);            \
    }

#endif