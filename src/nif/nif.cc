#include <cstring>

#include "nif.h"

nmap_t nmap[1000];
int nifs = 0;

const char *path = "path not set";

api nif_t *nif_get(void *key)
{
	for (int i = 0; i < nifs; i++)
	{
	if (nmap[i].key == key)
	{
	return nmap[i].value;
	}
	}
}

api void nif_add(void *key, nif_t *nif)
{
	nmap[nifs++] = nmap_t{key, nif};
}

api void nif_make(void *key, nif_t *nif)
{
	nif->n = nifs;

	

	nif_add(key, nif);
}
