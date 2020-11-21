#include "putc.h"

#include "nifp.h"
#include "nitypes.h"

#define Hedr   nif->hdr
#define Blocks nif->blocks
#define Skips  rd->skips

// rundown / visitor

void visit(struct nifprd *, int, int);
void visit_other(struct nifprd *, int, int);
void visit_block(struct nifprd *, void *);

api struct nifprd *malloc_nifprd() {
	struct nifprd *rd = malloc(sizeof(struct nifprd));
	memset(rd, 0, sizeof(struct nifprd));
	rd->other = visit_other;
	rd->ni_node = rd->ni_tri_shape
				= rd->ni_tri_shape_data
				= rd->bs_lighting_shader_property
				= rd->bs_shader_texture_set
				= visit_block;
	return rd;
}

api void free_nifprd(struct nifprd **p) {
	struct nifprd *rd = *p;
	free(rd->skips);
	free(rd);
	*p = NULL;
}

api void nifp_rd(struct nifp *nif, struct nifprd *rd) {
	// printf("nif rd\n");
	rd->skips = malloc(sizeof(char) * Hedr->num_blocks);
	memset(rd->skips, 0, sizeof(char) * Hedr->num_blocks);
	rd->nif = nif;
	for (int n = 0; n < Hedr->num_blocks; n++)
	visit(rd, -1, n);
}

static void visit(struct nifprd *rd, int parent, int current)
{
#define needs_parent if (-1 == parent) return;
	struct nifp *nif = rd->nif;
	if (-1 == current)
	return;
	if (rd->skips[current])
	return;
	rd->parent = parent; rd->current = current;
	// skip
	const char *block_type = Hedr->block_types[Hedr->block_type_index[current]];
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) )
	{
	Skips[current] = 1;
	struct ni_node_pointer *block_pointer = Blocks[current];
	rd->ni_node(rd, Blocks[current]);
	for (int i = 0; i < block_pointer->A->num_children; i++)
	{
	int b = block_pointer->children[i];
	visit(rd, current, b);
	}
	}
	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) )
	{
	Skips[current] = 1;
	struct ni_tri_shape_pointer *block_pointer = Blocks[current];
	rd->ni_tri_shape(rd, Blocks[current]);
	visit(rd, current, block_pointer->A->data);
	visit(rd, current, block_pointer->B->shader_property);
	visit(rd, current, block_pointer->B->alpha_property);
	}
	else if ( ni_is_type(NI_TRI_SHAPE_DATA) )
	{
	needs_parent;
	rd->ni_tri_shape_data(rd, Blocks[current]);
	}
	else if ( ni_is_type(BS_LIGHTING_SHADER_PROPERTY) )
	{
	needs_parent;
	struct bs_lighting_shader_property_pointer *block_pointer = Blocks[current];
	rd->bs_lighting_shader_property(rd, Blocks[current]);
	visit(rd, current, block_pointer->B->texture_set);
	}
	else if ( ni_is_type(BS_SHADER_TEXTURE_SET) )
	{
	needs_parent;
	rd->bs_shader_texture_set(rd, Blocks[current]);
	}
	else if ( ni_is_type(NI_ALPHA_PROPERTY) )
	{
	needs_parent;
	rd->ni_alpha_property(rd, Blocks[current]);
	}
	else if ( ni_is_any(NI_SKIN_INSTANCE, BS_DISMEMBER_SKIN_INSTANCE, NULL) )
	{
	needs_parent;
	rd->ni_skin_instance(rd, Blocks[current]);
	}
}

static void visit_other(struct nifprd *rd, int parent, int current) {}
static void visit_block(struct nifprd *rd, void *block_pointer) {}