#include "c/c.h"

#include "nif.h"

nmap_t nmap[1000];
int nifs = 0;

//const char *path = "path not set";

api nif_t *nif_alloc() {
	nif_t *bucket = malloc(sizeof(nif_t));
	bucket->pos = 0;
	bucket->buf = 0;
	return bucket;
}

api void nif_add(void *key, nif_t *nif)
{
	nif->n = nifs;
	nmap[nifs++] = (nmap_t){key, nif};
}

api nif_t *nif_get_stored(void *key)
{
	for (int i = 0; i < nifs; i++)
	{
	if (nmap[i].key == key)
	{
	return nmap[i].value;
	}
	}
	return 0;
}

api void nif_make(void *key, nif_t *nif)
{
	cassert_(nif->buf, "nif->buf NULL");
	nif_read_header(nif);
	nif_read_blocks(nif);
	nif_add(key, nif);
}
