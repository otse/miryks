#include "c.h"

#include "nif.h"
#include "bsa.h"

void test_visitor(int, int, nif_visitor_t *);

void nif_test(void *bsa)
{
	bsa_t *meshes = (bsa_t *)bsa;
	rc_t *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	cassert_(rc, "mh no bucket02a");
	bsa_read(meshes, rc);
	nif_t *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->inf;
	nif_read(bucket);
	nif_save(rc, bucket);
	nif_visitor_t *visitor = nif_alloc_visitor();
	visitor->nif = bucket;
    visitor->callback = test_visitor;
	nif_accept(bucket, visitor, NULL);
}

void test_visitor(int parent, int block, nif_visitor_t *visitor) {
	
}