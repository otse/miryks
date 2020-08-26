#ifndef C_FILES_H
#define C_FILES_H

#include <stdio.h>

int exists(const char *);
//char *fsamplevar(const char *, const char *, const char *);

char *FileStem(const char *, char);
char *FileName(const char *, char);
char *abf(const char *, const char *);

char *ReadFile(const char *);

char *cfout(const char *, const char *);
char *cfout2(const char *, const unsigned char *, int);

#endif