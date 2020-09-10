// stuff for elder scrolls plugin, bethesda archive and netimmerse file format

#ifndef LIB_PUTC_H
#define LIB_PUTC_H

#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define cassert(e, m) if (!(e)) Fail(m);

/*inline*/ int Fail(const char *);

//cfout("error.txt", m);

int exists(const char *);
//char *fsamplevar(const char *, const char *, const char *);

char *FileStem(char[260], const char *, char);
char *FileName(char[260], const char *, char);
char *abf(const char *, const char *);

char *ReadFile2(const char *);

char *cfout(const char *, const char *);
char *cfout2(const char *, const unsigned char *, int);

#endif