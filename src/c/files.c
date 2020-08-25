#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include "files.h"

int fchec(const char *p)
{
	struct stat buffer;
	return (stat(p, &buffer) == 0);
}

char *fstem(const char *p, char delim)
{
	const char *c = strrchr(p, delim);
	if (!c)
		return NULL;
	int l = (c - p) + 1;
	char *d = malloc(l * sizeof(char));
	strncpy(d, p, l);
	d[l - 1] = '\0';
	return d;
}

char *fname(const char *p, char delim)
{
	const char *c = strrchr(p, delim);
	if (!c)
		return NULL;
	c += 1;
	int l = strlen(p) - (c - p) + 1;
	char *d = malloc(l * sizeof(char));
	strncpy(d, c, l-1);
	d[l - 1] = '\0';
	return d;
}

char *fout(const char *n, const char *m)
{
	FILE *stream = fopen(n, "wb");
	fputs(m, stream);
	fclose(stream);
	return 0;
}

char *fout2(const char *n, const unsigned char *u, int size)
{
	FILE *stream = fopen(n, "wb");
	fwrite(u, 1, size, stream);
	fclose(stream);
	return 0;
}