#include "putc.h"

#include "nifp.h"
#include "bsa.h"

void test_callback(struct rd *, int, int);

// look at mesh.cc for good example of rd

void nifp_test()
{
	struct bsa *meshes = get_archives()[0];
	struct rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	cassert(rc, "mh no bucket02a");
	bsa_read(meshes, rc);
	struct nifp *bucket = malloc_nifp();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nif_read(bucket);
	//nif_save(rc, bucket);
	struct nifprd *rd = malloc_nifprd();
	rd->data = 0x1; // like a Mesh instance
    rd->other = test_callback;
	nifp_rd(bucket, rd, NULL);
    printf("nifptest ok\n");
}

static void test_callback(struct rd *rd, int parent, int block) {
	
}