// sources used
// https://stackoverflow.com/questions/3553296/sizeof-single-struct-member-in-c

// called nifp for nif but with pointers
// nifp.c does not replace nif.c

#include "putc.h"

#include "nif.h"

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks

#define Depos  Buf + Pos

#define nifpr struct nif *nif, int n, int before, int after

void sink_val(struct nif *nif, char *block_pointer, int src, int size) {
	char **dest = block_pointer + src;
	*dest = Depos;
	Pos += size;
	printf("sink val %i\n", size);
}

#define SinkVal(nif, block_pointer, layout, part) sink_val(nif, block_pointer, offsetof(layout, part), sizeof(((layout *)0)->part))
#define SinkArr(nif, block_pointer, layout, part) sink_val(nif, block_pointer, offsetof(layout, part), 0)

#define IncrArr(count, type) Pos += count * sizeof(type);

void *read_ni_alpha_property2(nifpr)
{
	printf("nifp exp\n");
	struct ni_alpha_property_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_alpha_property_pointer));
	Pos = before;
	//int of = offsetof(struct ni_alpha_property_pointer, A);
	//printf("offsetof(struct ni_alpha_property_pointer, A) %i\n", of);
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, A);
	printf("fields from A %i %u\n", block_pointer->A->name, block_pointer->A->num_extra_data_list);
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, B);
	IncrArr(block_pointer->A->num_extra_data_list, ni_ref);
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, C);
	printf("fields from B %i %hu %u\n", block_pointer->C->controller, block_pointer->C->flags, block_pointer->C->treshold);
	Pos = after;
	return block_pointer;

	sizeof(struct ni_alpha_property_pointer);
	sizeof(((struct ni_alpha_property_pointer *)0)->B);
	sizeof(struct ni_alpha_property);
}

/// END OF EXPERIMENTAL AREA