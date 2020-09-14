#include "putc.h"

#include "nif.h"
#include "bsa.h"

void test_callback(struct rd *, int, int);

// look at mesh.cc for good example of rd

void nif_test()
{
	struct bsa *meshes = get_archives()[0];
	struct rc *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	// or
	// struct rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	cassert(rc, "mh no bucket02a");
	bsa_read(meshes, rc);
	struct nif *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nif_read(bucket);
	//nif_save(rc, bucket);
	struct rd *rd = nif_alloc_rd();
	rd->data = 0x1; // Like some Geometry instance
    rd->other = test_callback;
	nif_rd(bucket, rd, NULL);
	printf("nif hedr end %i\n", bucket->hdr.end);
}

static void test_callback(struct rd *rd, int parent, int block) {
	
}