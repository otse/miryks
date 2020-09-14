// nif but with pointers

#include "putc.h"

#include "nifp.h"

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  Buf + Pos

#define FromBuf(x) *(x*)(Depos); Pos += sizeof(x);

static void sink_val(struct nifp *nif, char *block_pointer, int src, int size) {
	char **dest = block_pointer + src;
	*dest = Depos;
	Pos += size;
	//printf("sink val %i\n", size);
}

// sizeof(((layout *)0)->part)
#define SinkVal(nif, block_pointer, layout, part, size) sink_val(nif, block_pointer, offsetof(layout, part), size)

#define GetSize(count, type) count * sizeof(type)

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
	cassert(Buf, "nifp_read Buf not set");
	nifp_read_header(nif);
	//nifp_read_blocks(nif);
}

// begin hedr

static void read_short_string(struct nifp *nif, char **string) {
	char len = FromBuf(char);
	*string = malloc(sizeof(char) * len);
	strncpy(*string, Depos, len);
	Pos += len;
}

static void read_sized_string(struct nifp *nif, char **string) {
	unsigned int len = FromBuf(unsigned int);
	*string = malloc(sizeof(char) * len + 1);
	strncpy(*string, Depos, len);
	(*string)[len] = '\0';
	Pos += len;
}

static void hedr_read_header_string(struct nifp *nif) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(Buf, '\n') - Buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, Buf, n);
	string[n - 1] = '\0';
	Hedr.header_string = string;
	Hedr.version = string + 30;
	Pos += n;
}

static void read_value(struct nifp *nif, void *dest, int size) {
	memcpy(dest, Depos, size);
	Pos += size;
}

static void read_array(struct nifp *nif, void **dest, int size) {
	*dest = malloc(size);
	read_value(nif, *dest, size);
}

static void read_sized_strings(struct nifp *nif, char ***dest, int count) {
	*dest = malloc(count * sizeof(char *));
	for (int i = 0; i < count; i++)
	read_sized_string(nif, &(*dest)[i]);
}

api void nifp_read_header(struct nifp *nif) {
	hedr_read_header_string(nif);
	read_value(nif, &Hedr.unknown_1, 17);
	read_short_string(nif, &Hedr.author);
	read_short_string(nif, &Hedr.process_script);
	read_short_string(nif, &Hedr.export_script);
	printf("export script %s\n", Hedr.export_script);
	read_value(nif, &Hedr.num_block_types, 2);
	read_sized_strings(nif, &Hedr.block_types, Hedr.num_block_types);
	read_array(nif, &Hedr.block_type_index, sizeof(unsigned short) * Hedr.num_blocks);
	read_array(nif, &Hedr.block_sizes,      sizeof(unsigned int)   * Hedr.num_blocks);
	read_value(nif, &Hedr.num_strings, 4);
	read_sized_strings(nif, &Hedr.strings, Hedr.num_strings);
	read_value(nif, &Hedr.num_groups, 4);
	Hedr.end = Pos;
}

#define nifpr struct nifp *nif, int n, int before, int after

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