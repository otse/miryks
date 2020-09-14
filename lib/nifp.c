// nif but with pointers

#include "putc.h"

#include "nifp.h"

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  Buf + Pos

#define FromBuf(x) *(x*)(Depos); Pos += sizeof(x);

api char *nifp_get_string(struct nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr.strings[i];
}

api char *nifp_get_block_type(struct nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr.block_types[Hedr.block_type_index[i]];
}

api void *nifp_get_block(struct nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Blocks[i];
}

api struct nifp *malloc_nifp() {
	struct nifp *nif = malloc(sizeof(struct nifp));
	Pos = 0;
	Buf = 0;
	return nif;
}

api void nifp_read(struct nifp *nif) {
	cassert(Buf, "nif_read Buf not set");
	nif_read_header(nif);
	nif_read_blocks(nif);
}

// todo need hard rewrite of hedr section!

static void hedr_read_header_string(struct nifp *);
static void hedr_read_vars(struct nifp *);
static void hedr_read_block_types(struct nifp *);
static void hedr_read_block_type_index(struct nifp *);
static void hedr_read_block_sizes(struct nifp *);
static void hedr_read_strings(struct nifp *);
static void hedr_read_groups(struct nifp *);

static  char *nifp_read_short_string(struct nifp *nif) {
	char len = FromBuf(char);
	char *string = malloc(sizeof(char) * len);
	strncpy(string, Depos, len);
	Pos += len;
	return string;
}

static  char *nifp_read_sized_string(struct nifp *nif) {
	int len = FromBuf(int);
	char *string = malloc(sizeof(char) * len + 1);
	strncpy(string, Depos, len);
	string[len] = '\0';
	Pos += len;
	return string;
}

api void nifp_read_header(struct nifp *nif) {
	hedr_read_header_string(nif);
	Hedr.unknown_1 = FromBuf(int);
	hedr_read_vars(nif);
	hedr_read_block_types(nif);
	hedr_read_block_type_index(nif);
	hedr_read_block_sizes(nif);
	hedr_read_strings(nif);
	hedr_read_groups(nif);
	Hedr.end = Pos;
}

void hedr_read_header_string(struct nifp *nif) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(Buf, '\n') - Buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, Buf, n);
	string[n - 1] = '\0';
	Hedr.header_string = string;
	Hedr.version = string + 30;
	Pos += n;
}

void hedr_read_vars(struct nifp *nif) {
	Hedr.endian_type = FromBuf(unsigned char);
	Hedr.user_value =  FromBuf(unsigned int);
	Hedr.num_blocks =  FromBuf(unsigned int);
	Hedr.user_value_2 =FromBuf(unsigned int);
	Hedr.author =        nifp_read_short_string(nif);
	Hedr.process_script =nifp_read_short_string(nif);
	Hedr.export_script = nifp_read_short_string(nif);
	Hedr.num_block_types = FromBuf(unsigned short);
}

void hedr_read_block_types(struct nifp *nif) {
	int n = Hedr.num_block_types;
	Hedr.block_types = malloc(sizeof(char *) * n);
	for (int i = 0; i < n; i++)
	{
	Hedr.block_types[i] = nif_read_sized_string(nif);
	}
}

void hedr_read_block_type_index(struct nifp *nif) {
	int size = sizeof(unsigned short) * Hedr.num_blocks;
	Hedr.block_type_index = malloc(size);
	memcpy(Hedr.block_type_index, Depos, size);
	Pos += size;
}

void hedr_read_block_sizes(struct nifp *nif) {
	int size = sizeof(unsigned int) * Hedr.num_blocks;
	Hedr.block_sizes = malloc(size);
	memcpy(Hedr.block_sizes, Depos, size);
	Pos += size;
}

void hedr_read_strings(struct nifp *nif) {
	Hedr.num_strings = FromBuf(unsigned int);
	Hedr.max_string_length = FromBuf(unsigned int);
	int n = Hedr.num_strings;
	Hedr.strings = malloc(sizeof(char *) * n);
	for (int i = 0; i < n; i++)
	{
	Hedr.strings[i] = nif_read_sized_string(nif);
	}
}

void hedr_read_groups(struct nifp *nif) {
	Hedr.num_groups = FromBuf(unsigned int);
}

#define nifpr struct nifp *nif, int n, int before, int after

void sink_val(struct nifp *nif, char *block_pointer, int src, int size) {
	char **dest = block_pointer + src;
	*dest = Depos;
	Pos += size;
	printf("sink val %i\n", size);
}

// sizeof(((layout *)0)->part)

#define SinkVal(nif, block_pointer, layout, part, size) sink_val(nif, block_pointer, offsetof(layout, part), size)

#define GetSize(count, type) count * sizeof(type)

void *read_ni_alpha_property2(nifpr)
{
	printf("nifp exp\n");
	struct ni_alpha_property_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_alpha_property_pointer));
	Pos = before;
	//int of = offsetof(struct ni_alpha_property_pointer, A);
	//printf("offsetof(struct ni_alpha_property_pointer, A) %i\n", of);
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, A, 8);
	printf("fields from A %i %u\n", block_pointer->A->name, block_pointer->A->num_extra_data_list);
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, B, GetSize(block_pointer->A->num_extra_data_list, ni_ref));
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, C, 7);
	printf("fields from B %i %hu %u\n", block_pointer->C->controller, block_pointer->C->flags, block_pointer->C->treshold);
	Pos = after;
	return block_pointer;

	sizeof(struct ni_alpha_property_pointer);
	sizeof(((struct ni_alpha_property_pointer *)0)->C);
}

/// END OF EXPERIMENTAL AREA