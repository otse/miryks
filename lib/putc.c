#include "putc.h"

#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

/*inline*/ int Fail(const char *m)
{
	printf("Program Exiting: %s\n", m);
	system("PAUSE");
	exit(1);
}

int exists(const char *p)
{
	struct stat buffer;
	return (stat(p, &buffer) == 0);
}

char *FileStem(char s[260], const char *p, char delim)
{
	const char *c = strrchr(p, delim);
	if (!c)
		return NULL;
	int l = (c - p) + 1;
	strncpy(s, p, l);
	s[l - 1] = '\0';
	return s;
}

char *FileName(char s[260], const char *p, char delim)
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

char *ReadFile2(const char *filename)
{
	char *buffer = NULL;
	int string_size, read_size;
	FILE *handler = fopen(filename, "r");

	if (handler)
	{
		// Seek the last byte of the file
		fseek(handler, 0, SEEK_END);
		// Offset from the first to the last byte, or in other words, filesize
		string_size = ftell(handler);
		// go back to the start of the file
		rewind(handler);

		// Allocate a string that can hold it all
		buffer = (char *)malloc(sizeof(char) * (string_size + 1));

		// Read it all in one operation
		read_size = fread(buffer, sizeof(char), string_size, handler);

		// fread doesn't set it so put a \0 in the last position
		// and buffer is now officially a string
		buffer[string_size] = '\0';

		if (string_size != read_size)
		{
			// Something went wrong, throw away the memory and set
			// the buffer to NULL
			free(buffer);
			buffer = NULL;
		}

		// Always remember to close the file.
		fclose(handler);
	}

	return buffer;
}

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