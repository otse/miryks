#include "common.h"

// nif with pointers rundown

#include "nifp.h"
#include "nitypes.h"

#define Hedr   nif->hdr
#define Blocks nif->blocks
#define Skips  rd->skips

void visit      (NifpRd *, int, int);
void visit_other(NifpRd *, int, int);
void visit_block(NifpRd *, void *);

api NifpRd *calloc_nifprd() {
	NifpRd *rd = calloc(1, sizeof(NifpRd));
	rd->other = visit_other;
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
	assertm(rd->nif, "nifprd nif not set");
	rd->skips = calloc(Hedr->num_blocks, sizeof(char));
	//rd->nif = nif;
	for (int n = 0; n < Hedr->num_blocks; n++)
	visit(rd, -1, n);
}

#define needs_parent if (-1 == parent) return;

// very very horrible function
static void visit(NifpRd *rd, int parent, int current)
{
	Nifp *nif = rd->nif;
	if (-1 == current || rd->skips[current])
	return;
	rd->parent = parent; rd->current = current;
	// skip
	const char *block_type = Hedr->block_types[Hedr->block_type_index[current]];
	if (0) ;

	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) )
	{
		Skips[current] = 1;
		struct ni_node_pointer *block = Blocks[current];
		if (rd->ni_node_callback)
			rd->ni_node_callback(rd, Blocks[current]);
		for (int i = 0; i < block->A->num_children; i++)
		{
		int b = block->children[i];
		visit(rd, current, b);
		}
	}

	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) )
	{
		Skips[current] = 1;
		struct ni_tri_shape_pointer *block = Blocks[current];
		if (rd->ni_tri_shape_callback)
			rd->ni_tri_shape_callback(rd, Blocks[current]);
		if (block->A->skin_instance == -1)
			visit(rd, current, block->A->data);
		else
			visit(rd, current, block->A->skin_instance);
		visit(rd, current, block->B->shader_property);
		visit(rd, current, block->B->alpha_property);
	}

	else if ( ni_is_type(NI_TRI_SHAPE_DATA) )
	{
		needs_parent
		if (rd->ni_tri_shape_data_callback)
			rd->ni_tri_shape_data_callback(rd, Blocks[current]);
	}

	special_edition
	else if ( ni_is_type(BS_TRI_SHAPE) )
	{
		needs_parent
		Skips[current] = 1;
		struct bs_tri_shape_pointer *block = Blocks[current];
		if (rd->bs_tri_shape_callback)
			rd->bs_tri_shape_callback(rd, Blocks[current]);
		visit(rd, current, block->refs->shader_property);
		visit(rd, current, block->refs->alpha_property);
	}

	else if ( ni_is_type(BS_LIGHTING_SHADER_PROPERTY) )
	{
		needs_parent
		struct bs_lighting_shader_property_pointer *block = Blocks[current];
		if (rd->bs_lighting_shader_property_callback)
			rd->bs_lighting_shader_property_callback(rd, Blocks[current]);
		visit(rd, current, block->B->texture_set);
	}

	else if ( ni_is_type(BS_EFFECT_SHADER_PROPERTY) )
	{
		needs_parent
		struct bs_effect_shader_property_pointer *block = Blocks[current];
		if (rd->bs_effect_shader_property_callback)
			rd->bs_effect_shader_property_callback(rd, Blocks[current]);
	}

	else if ( ni_is_type(BS_SHADER_TEXTURE_SET) )
	{
		needs_parent
		if (rd->bs_shader_texture_set_callback)
			rd->bs_shader_texture_set_callback(rd, Blocks[current]);
	}

	else if ( ni_is_type(NI_ALPHA_PROPERTY) )
	{
		needs_parent
		if (rd->ni_alpha_property_callback)
			rd->ni_alpha_property_callback(rd, Blocks[current]);
	}

	else if ( ni_is_any(NI_SKIN_INSTANCE, BS_DISMEMBER_SKIN_INSTANCE, NULL) )
	{
		needs_parent
		struct ni_skin_instance_pointer *block = Blocks[current];
		if (rd->ni_skin_instance_callback)
			rd->ni_skin_instance_callback(rd, Blocks[current]);
		visit(rd, current, block->A->data);
		visit(rd, current, block->A->skin_partition);
		//visit(rd, current, block->A->skeleton_root);
	}

	else if ( ni_is_type(NI_SKIN_DATA) )
	{
		needs_parent
		if (rd->ni_skin_data_callback)
			rd->ni_skin_data_callback(rd, Blocks[current]);
	}

	else if ( ni_is_type(NI_SKIN_PARTITION) )
	{
		needs_parent
		if (rd->ni_skin_partition_callback)
			rd->ni_skin_partition_callback(rd, Blocks[current]);
	}
	
	else
	{
		if (rd->other)
			rd->other(rd, Blocks[current]);
	}
}

static void visit_other(NifpRd *rd, void *block) {}
static void visit_block(NifpRd *rd, void *block) {}