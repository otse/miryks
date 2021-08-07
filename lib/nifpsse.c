#include "common.h"

#include "nifp.h"

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  Buf + Pos

static void sink_val(Nifp *nif, char *block_pointer, int src, int size) {
	char **dest = block_pointer + src;
	*dest = Depos;
	Pos += size;
}

// sizeof(((layout *)0)->part)
#define Sink(nif, block_pointer, layout, part, size) sink_val(nif, (char *)block_pointer, offsetof(struct layout, part), size)
#define Arr(count, type) count * sizeof(type)

void *read_ni_common_layout(Nifp *nif, int n);

void *read_bs_tri_shape(Nifp *nif, int n)
{
	printf("read bs tri shape\n");
	struct bs_tri_shape_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct bs_tri_shape_pointer));
	memset(block_pointer, 0, sizeof(struct bs_tri_shape_pointer));
	block_pointer->common = read_ni_common_layout(nif, n);
	Sink(nif, block_pointer, bs_tri_shape_pointer, bounding_sphere, 16);
	Sink(nif, block_pointer, bs_tri_shape_pointer, B, 4);
	Sink(nif, block_pointer, bs_tri_shape_pointer, bs_properties, 8);
	// sizeof(struct bs_vertex_data_sse);
	// sizeof(((struct bs_tri_shape_pointer *)0)->C)
	Sink(nif, block_pointer, bs_tri_shape_pointer, C, 16);
	printf("c-> num vertices is %u\n", block_pointer->C->num_vertices);

	Sink(nif, block_pointer, bs_tri_shape_pointer, vertex_data,
		Arr(block_pointer->C->num_vertices, struct bs_vertex_data_sse));

	struct bs_vertex_data_sse *zero = &block_pointer->vertex_data[1];
	printf("bs_vertex_data_sse[0] vertex x is %f\n", zero->vertex.x);
	printf("bs_vertex_data_sse[0] bitangent x is %f\n", zero->bitangent_x);
	printf("bs_vertex_data_sse[0] bitangent y is %u\n", zero->bitangent_y);
	printf("bs_vertex_data_sse[0] bitangent z is %u\n", zero->bitangent_z);

	Sink(nif, block_pointer, bs_tri_shape_pointer, triangles,
		Arr(block_pointer->C->num_triangles, 6));

	Sink(nif, block_pointer, bs_tri_shape_pointer, D, 4);
	printf("particle data size %u\n", block_pointer->D->particle_data_size);
	return block_pointer;
}