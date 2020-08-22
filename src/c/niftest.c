#include "c.h"

#include "nif.h"
#include "bsa.h"

void test_visit_ni_node(int);

void nif_test(void *bsa)
{
	bsa_t *meshes = (bsa_t *)bsa;
	rc_t *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	cassert_(rc, "mh no bucket02a");
	bsa_read(meshes, rc);
	nif_t *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->inf;
	nif_read(rc, bucket);
	nif_visitor_t *visitor = nif_alloc_visitor();
    visitor->ni_node = test_visit_ni_node;
	nif_accept(bucket, visitor);
}

void test_visit_ni_node(int x) {
	printf("niftest.c visit ni node\n");
}