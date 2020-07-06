#include <cstring>

#include "nif"
#include "../bsa/bsa"


struct map_t {
	const char *key;
	nif_t *value;
} map[1000];

static int nifs = 0;

api nif_t *nif_get(const char *p)
{
	for (int i = 0; i < nifs; i++)
	{
	if (!strcmp(map[i].key, p))
	{
	return map[i].value;
	}
	}
}

api void nif_add(const char *p, nif_t *nif)
{
	map[nifs++] = map_t{p, nif};
}

api void nif_make(rc_t *rc, nif_t *nif)
{
	nif_add(rc->path, nif);
}
