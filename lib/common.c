
#include <stdlib.h>
#include <string.h>

#include "common.h"

int pause(const char *m)
{
	printf("Program Exiting: %s\n", m);
	system("PAUSE");
	exit(1);
}

#include <sys/stat.h>

int exists(const char *p)
{
	struct stat buffer;
	return (stat(p, &buffer) == 0);
}

// file stem and name are used by bsa.c

char *file_stem(char s[260], const char *p, char delim)
{
	const char *c = strrchr(p, delim);
	if (!c)
		return NULL;
	int l = (c - p) + 1;
	strncpy(s, p, l);
	s[l - 1] = '\0';
	return s;
}

char *file_name(char s[260], const char *p, char delim)
{
	const char *c = strrchr(p, delim);
	if (!c)
		return NULL;
	c += 1;
	int l = strlen(p) - (c - p) + 1;
	strncpy(s, c, l - 1);
	s[l - 1] = '\0';
	return s;
}

char *abf(const char *a, const char *b)
{
	char *ab = malloc(strlen(a) + strlen(b) + 2);
	strcpy(ab, a);
	strcat(ab, "\\");
	strcat(ab, b);
	return ab;
}

/*
char *fsamplevar(const char *a, const char *b, const char *c)
{
	char *has_var = strstr(a, b);
	if (!has_var)
	return c;
}
*/

char *cfout(const char *n, const char *m)
{
	FILE *stream = fopen(n, "wb");
	fputs(m, stream);
	fclose(stream);
	return 0;
}

char *cfout2(const char *n, const unsigned char *u, int size)
{
	FILE *stream = fopen(n, "wb");
	fwrite(u, 1, size, stream);
	fclose(stream);
	return 0;
}