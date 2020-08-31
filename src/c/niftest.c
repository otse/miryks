#include "c.h"

#include "nif.h"
#include "bsa.h"

void test_callback(rd_t *, int, int);

// look at mesh.cc for good example of rd

void nif_test(bsa_t *meshes)
{
	rc_t *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	cassert_(rc, "mh no bucket02a");
	bsa_read(meshes, rc);
	nif_t *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nif_read(bucket);
	//nif_save(rc, bucket);
	rd_t *rd = nif_alloc_rd();
	rd->data = 0x1; // Like some Geometry instance
    rd->other = test_callback;
	nif_rd(bucket, rd, NULL);
}

void test_callback(rd_t *rd, int parent, int block) {
	
}