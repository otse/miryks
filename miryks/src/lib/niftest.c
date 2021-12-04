#include <lib/common.h>

#include <lib/nif.h>
#include <lib/bsa.h>

void test_callback(RD, int, int);
static void test_bucket_against_known_values(NIF);

// look at mesh.cc for good example of rd

void nif_test()
{
	// We use Res from bsa.h
	RES res = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	// or
	// struct bsa *meshes = get_archives()[0];
	// struct RES rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	assertm(res, "mh no bucket02a");
	bsa_read(res);
	// setup
	NIF bucket = calloc_nifp();
	bucket->path = res->path;
	bucket->buf = res->buf;
	nif_read(bucket);
	// save_nif(rc, bucket);
	// visitor
	RD rd = calloc_nifprd();
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

static void test_callback(RD rd, int parent, int current)
{
}