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

void sink_val(struct nif *nif, char *block_pointer, int src) {
	char **dest = block_pointer + src;
	*dest = Depos;
}

#define SinkVal(nif, block_pointer, layout, part) \
	sink_val(nif, block_pointer, offsetof(layout, part))

void *read_ni_alpha_property2(nifpr)
{
	printf("nifp exp");
	struct ni_alpha_property_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_alpha_property_pointer));
	Pos = before;
	//int of = offsetof(struct ni_alpha_property_pointer, A);
	//printf("offsetof(struct ni_alpha_property_pointer, A) %i\n", of);
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, A);
	printf("requiest field name %i %u\n", block_pointer->A->name, block_pointer->A->num_extra_data_list);
	Pos = after;
	return block_pointer;

	sizeof(struct ni_alpha_property_pointer);
}

/// END OF EXPERIMENTAL AREA