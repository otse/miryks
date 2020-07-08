#ifndef IDIOM_H
#define IDIOM_H

//extern "C" {
#include "c/files.h"
//}

#define ERTXT "error.txt"

void log_(const char *p) {
    printf("%s\n", p);
}

#define assert_(e, m)       \
    if (!(e))               \
    {                       \
        fout(ERTXT, m);     \
        exit(1); \
    }

#endif