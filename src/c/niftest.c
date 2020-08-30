#include "c.h"

#include "nif.h"
#include "bsa.h"

void test_callback(Rd *, int, int);

// look at mesh.cc for good example of rd

void nif_test(Bsa *meshes)
{
	Rc *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	cassert_(rc, "mh no bucket02a");
	bsa_read(meshes, rc);
	Nif *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nif_read(bucket);
	//nif_save(rc, bucket);
	Rd *rd = nif_alloc_rd();
	rd->data = 0x1; // Like some Geometry instance
    rd->other = test_callback;
	nif_rd(bucket, rd, NULL);
}

void test_callback(Rd *rd, int parent, int block) {
	
}