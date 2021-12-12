#include <lib/common.h>

#include <lib/nif.h>
#include <lib/bsa.h>

void test_callback(RD, int, int);
static void test_bucket_against_known_values(NIF);

// look at model.cpp for good example of rd

void nif_test()
{
	// We use RES from bsa.h
	resource *res = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	// or
	// struct bsa *meshes = get_archives()[0];
	// struct RES res = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	assertm(res, "mh no bucket02a");
	bsa_read(res);
	// setup
	nif *bucket = calloc_ni();
	bucket->path = res->path;
	bucket->buf = res->buf;
	nif_read(bucket);
	// save_nif(rc, bucket);
	// visitor
	rundown *rd = calloc_rd();
	rd->ni = bucket;
	rd->data = 0xf; // like a Mesh instance
	rd->other = test_callback;
	nif_rd(rd);
	// cleanup
	free_rd(&rd);
	free_ni(&bucket);
	//char str[600];
	//nif_print_hedr(bucket, str);
	//printf("nifp hedr end %i\n", bucket->hdr->end);
}

static void test_callback(RD rd, int parent, int current)
{
}