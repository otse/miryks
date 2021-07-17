#include "putc.h"

#include "nifp.h"

#include <bsa.h>

void test_callback(NifpRd *, int, int);
static void test_bucket_against_known_values(Nifp *);

// look at mesh.cc for good example of rd

void nifp_test()
{
	return;
	// We use Rc from bsa.h
	Rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	// or
	// struct bsa *meshes = get_archives()[0];
	// struct Rc *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	cassert(rc, "mh no bucket02a");
	bsa_read(rc);
	// Basic
	Nifp *bucket = malloc_nifp();
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	nifp_read(bucket);
	nifp_save(rc, bucket);
	// Visitor
	NifpRd *rd = malloc_nifprd();
	rd->nif = bucket;
	rd->data = 0xf; // like a Mesh instance
	rd->other = test_callback;
	nifp_rd(rd);
	// Cleanup
	free_nifprd(&rd);
	free_nifp(&bucket);
	//char str[600];
	//nifp_print_hedr(bucket, str);
	//printf("nifp hedr end %i\n", bucket->hdr->end);
}

static void test_callback(NifpRd *rd, int parent, int current)
{
}