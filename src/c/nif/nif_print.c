#include "c/c.h"

#include "nif.h"

char *nif_print_hedr(nif_t *nif)
{
	char *buf = malloc(5 * sizeof(char));
	*buf = "boo0\0";
	return buf;
}
