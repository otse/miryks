#include "common.h"
#include "nif.h"
#include "bsa.h"

void test_callback(Rd *, int, int);
static void test_bucket_against_known_values(Nif *);

// look at mesh.cc for good example of rd

void nif_test()
{
	// We use Rsc from bsa.h
	Rsc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	// or
	// struct bsa *meshes = get_archives()[0];
	// struct Rsc *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	assertm(rc, "mh no bucket02a");
	bsa_read(rc);
	// setup
	Nif *bucket = calloc_nifp();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nif_read(bucket);
	// ext_nif_save(rc, bucket);
	// visitor
	Rd *rd = calloc_nifprd();
	rd->nif = bucket;
	rd->data = 0xf; // like a Mesh instance
	rd->other = test_callback;
	nif_rd(rd);
	// cleanup
	free_nifprd(&rd);
	free_nifp(&bucket);
	//char str[600];
	//nif_print_hedr(bucket, str);
	//printf("nifp hedr end %i\n", bucket->hdr->end);
}

static void test_callback(Rd *rd, int parent, int current)
{
}