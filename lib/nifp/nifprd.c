#include "putc.h"

#include "nifp.h"
#include "nitypes.h"

#define Hedr   nif->hdr
#define Blocks nif->blocks
#define Skips  rd->skips

// rundown / visitor

void visit      (NifpRd *, int, int);
void visit_other(NifpRd *, int, int);
void visit_block(NifpRd *, void *);

api NifpRd *malloc_nifprd() {
	NifpRd *rd = malloc(sizeof(NifpRd));
	memset(rd, 0, sizeof(NifpRd));
	rd->other = visit_other;
	/*rd->ni_node = rd->ni_tri_shape
				= rd->ni_tri_shape_data
				= rd->bs_lighting_shader_property
				= rd->bs_shader_texture_set
				= rd->ni_alpha_property
				= rd->ni_controller_sequence
				= rd->ni_skin_instance
				= visit_block;*/
	return rd;
}

api void free_nifprd(NifpRd **p) {
	NifpRd *rd = *p;
	free(rd->skips);
	free(rd);
	*p = NULL;
}

api void nifp_rd(NifpRd *rd) {
	// printf("nif rd\n");
	Nifp *nif = rd->nif;
	cassert(rd->nif, "nifprd nif not set");
	rd->skips = malloc(sizeof(char) * Hedr->num_blocks);
	memset(rd->skips, 0, sizeof(char) * Hedr->num_blocks);
	//rd->nif = nif;
	for (int n = 0; n < Hedr->num_blocks; n++)
	visit(rd, -1, n);
}

static void visit(NifpRd *rd, int parent, int current)
{
#define needs_parent if (-1 == parent) return;
	Nifp *nif = rd->nif;
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
	if (rd->ni_node)
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
	if (rd->ni_tri_shape)
		rd->ni_tri_shape(rd, Blocks[current]);
	if (block_pointer->A->skin_instance == -1)
		visit(rd, current, block_pointer->A->data);
	else
		visit(rd, current, block_pointer->A->skin_instance);
	visit(rd, current, block_pointer->B->shader_property);
	visit(rd, current, block_pointer->B->alpha_property);
	}
	else if ( ni_is_type(NI_TRI_SHAPE_DATA) )
	{
	needs_parent;
	if (rd->ni_tri_shape_data)
		rd->ni_tri_shape_data(rd, Blocks[current]);
	}
	else if ( ni_is_type(BS_LIGHTING_SHADER_PROPERTY) )
	{
	needs_parent;
	struct bs_lighting_shader_property_pointer *block_pointer = Blocks[current];
	if (rd->bs_lighting_shader_property)
		rd->bs_lighting_shader_property(rd, Blocks[current]);
	visit(rd, current, block_pointer->B->texture_set);
	}
	else if ( ni_is_type(BS_EFFECT_SHADER_PROPERTY) )
	{
	needs_parent;
	struct bs_effect_shader_property_pointer *block_pointer = Blocks[current];
	if (rd->bs_effect_shader_property)
		rd->bs_effect_shader_property(rd, Blocks[current]);
	}
	else if ( ni_is_type(BS_SHADER_TEXTURE_SET) )
	{
	needs_parent;
	if (rd->bs_shader_texture_set)
		rd->bs_shader_texture_set(rd, Blocks[current]);
	}
	else if ( ni_is_type(NI_ALPHA_PROPERTY) )
	{
	needs_parent;
	if (rd->ni_alpha_property)
		rd->ni_alpha_property(rd, Blocks[current]);
	}
	else if ( ni_is_any(NI_SKIN_INSTANCE, BS_DISMEMBER_SKIN_INSTANCE, NULL) )
	{
	needs_parent;
	struct ni_skin_instance_pointer *block_pointer = Blocks[current];
	if (rd->ni_skin_instance)
		rd->ni_skin_instance(rd, Blocks[current]);
	visit(rd, current, block_pointer->A->data);
	visit(rd, current, block_pointer->A->skin_partition);
	//visit(rd, current, block_pointer->A->skeleton_root);
	}
	else if ( ni_is_type(NI_SKIN_DATA) )
	{
	needs_parent;
	if (rd->ni_skin_data)
		rd->ni_skin_data(rd, Blocks[current]);
	}
	else if ( ni_is_type(NI_SKIN_PARTITION) )
	{
	needs_parent;
	if (rd->ni_skin_partition)
		rd->ni_skin_partition(rd, Blocks[current]);
	}
	else
	{
	if (rd->other)
		rd->other(rd, Blocks[current]);
	}
}

static void visit_other(NifpRd *rd, void *block_pointer) {}
static void visit_block(NifpRd *rd, void *block_pointer) {}