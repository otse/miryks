#include "c/c.h"

#include "nif.h"
#include "../bsa/bsa.h"

void nif_test(void *bsa)
{
	bsa_t *meshes = (bsa_t *)bsa;
	rc_t *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	assert_(rc, "cant find bucket");
	bsa_read(meshes, rc);
	nif_t *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nif_make(rc, bucket);
}