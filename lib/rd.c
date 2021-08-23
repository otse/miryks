#include "common.h"

/// nif rundown

#include "nif.h"
#include "niftypes.h"

#define Hedr   nif->hdr
#define Blocks nif->blocks
#define Skips  rd->skips

void visit (Rd *, int, int);

api Rd *calloc_nifprd() {
	Rd *rd = calloc(1, sizeof(Rd));
	return rd;
}

api void free_nifprd(Rd **p) {
	Rd *rd = *p;
	free(rd->skips);
	free(rd);
	*p = NULL;
}

api void nif_rd(Rd *rd) {
	// printf("nif rd\n");
	Nif *nif = rd->nif;
	assertm(rd->nif!=NULL, "nifrd nif not set");
	rd->skips = calloc(Hedr->num_blocks, sizeof(char));
	//rd->nif = nif;
	for (int n = 0; n < Hedr->num_blocks; n++)
	visit(rd, -1, n);
}

#define needs_parent if (-1 == parent) return;
#define traverse_once Skips[current] = 1;

// needs rewriting some time

static void visit(Rd *rd, int parent, int current)
{
	Nif *nif = rd->nif;
	if (-1 == current || rd->skips[current])
	return;
	rd->parent = parent; rd->current = current;
	// skip
	const char *block_type = Hedr->block_types[Hedr->block_type_index[current]];
	if (0) ;

	else if ( nif_types(NiNodeS, BSLeafAnimNodeS, BSFadeNodeS) )
	{
		traverse_once
		NiNode *block = Blocks[current];
		if (rd->ni_node_callback)
			rd->ni_node_callback(rd, Blocks[current]);
		for (unsigned int i = 0; i < block->A->num_children; i++)
		{
		int b = block->children[i];
		visit(rd, current, b);
		}
	}

	else if ( nif_types(NiTriShapeS, BSLODTriShapeS, NULL) )
	{
		traverse_once
		NiTriShape *block = Blocks[current];
		if (rd->ni_tri_shape_callback)
			rd->ni_tri_shape_callback(rd, Blocks[current]);
		if (block->A->skin_instance == -1)
			visit(rd, current, block->A->data);
		else
			visit(rd, current, block->A->skin_instance);
		visit(rd, current, block->B->shader_property);
		visit(rd, current, block->B->alpha_property);
	}

	else if ( nif_type(NiTriShapeDataS) )
	{
		needs_parent
		if (rd->ni_tri_shape_data_callback)
			rd->ni_tri_shape_data_callback(rd, Blocks[current]);
	}

	special_edition
	else if ( nif_type(BSTriShapeS) )
	{
		needs_parent
		traverse_once
		BSTriShape *block = Blocks[current];
		if (rd->bs_tri_shape_callback)
			rd->bs_tri_shape_callback(rd, Blocks[current]);
		visit(rd, current, block->refs->shader_property);
		visit(rd, current, block->refs->alpha_property);
		visit(rd, current, block->refs->skin);
	}

	else if ( nif_type(BSLightingShaderPropertyS) )
	{
		needs_parent
		BSLightingShaderProperty *block = Blocks[current];
		if (rd->bs_lighting_shader_property_callback)
			rd->bs_lighting_shader_property_callback(rd, Blocks[current]);
		visit(rd, current, block->B->texture_set);
	}

	else if ( nif_type(BSEffectShaderPropertyS) )
	{
		needs_parent
		BSEffectShaderProperty *block = Blocks[current];
		block->meta.parent = rd->parent;
		if (rd->bs_effect_shader_property_callback)
			rd->bs_effect_shader_property_callback(rd, Blocks[current]);
		visit(rd, current, block->B->controller);
	}

	else if ( nif_type(BSEffectShaderPropertyFloatControllerS) )
	{
		needs_parent
		BSEffectShaderPropertyFloatController *block = Blocks[current];
		if (rd->bs_effect_shader_property_float_controller_callback)
			rd->bs_effect_shader_property_float_controller_callback(rd, Blocks[current]);
		visit(rd, current, block->A->next_controller);
	}

	else if ( nif_type(NiFloatInterpolatorS) )
	{
		needs_parent
		NiFloatInterpolator *block = Blocks[current];
		if (rd->ni_float_interpolator_callback)
			rd->ni_float_interpolator_callback(rd, Blocks[current]);
	}

	else if ( nif_type(NiFloatDataS) )
	{

	}

	else if ( nif_type(BSShaderTextureSetS) )
	{
		needs_parent
		if (rd->bs_shader_texture_set_callback)
			rd->bs_shader_texture_set_callback(rd, Blocks[current]);
	}

	else if ( nif_type(NiAlphaPropertyS) )
	{
		needs_parent
		if (rd->ni_alpha_property_callback)
			rd->ni_alpha_property_callback(rd, Blocks[current]);
	}

	else if ( nif_types(NiSkinInstanceS, BSDismemberSkinInstanceS, NULL) )
	{
		needs_parent
		NiSkinInstance *block = Blocks[current];
		if (rd->ni_skin_instance_callback)
			rd->ni_skin_instance_callback(rd, Blocks[current]);
		visit(rd, current, block->A->data);
		visit(rd, current, block->A->skin_partition);
		//visit(rd, current, block->A->skeleton_root);
	}

	else if ( nif_type(NiSkinDataS) )
	{
		needs_parent
		if (rd->ni_skin_data_callback)
			rd->ni_skin_data_callback(rd, Blocks[current]);
	}

	else if ( nif_type(NiSkinPartitionS) )
	{
		needs_parent
		if (rd->ni_skin_partition_callback)
			rd->ni_skin_partition_callback(rd, Blocks[current]);
	}
	
	else
	{
		// catch all for unknown blocks
		if (rd->other)
			rd->other(rd, Blocks[current]);
	}
}
