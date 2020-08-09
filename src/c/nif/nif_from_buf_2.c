#include "c/c.h"

#include "nif.h"

#include "buf_helpers.h"

#define readreadparams nif_t *nif, int n

void read_ni_node(readparams);
void read_ni_skin_instance(readparams);
void read_ni_skin_data(readparams);

void nif_read_blocks(nif_t *nif)
{
	nif->blocks = malloc(sizeof(nif_block_t *) * hedr.num_blocks);
	for (int i = 0; i < hedr.num_blocks; i++)
	{
	nif->blocks[i] = malloc(sizeof(nif_block_t));
	nif_read_block(nif, i);
	}
    
}

#define type(x) 0 == strcmp(block_type, x)

void nif_read_block(nif_t *nif, int n)
{
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	if (0) ;
	else if (type(NI_NODE)) read_ni_node(nif, n);
	else if (type(BS_LEAF_ANIM_NODE)) read_ni_node(nif, n);
	else if (type(BS_FADE_NODE)) read_ni_node(nif, n);
	else if (type(NI_SKIN_INSTANCE)) read_ni_skin_instance(nif, n);
	else if (type(BS_DISMEMBER_SKIN_INSTANCE)) read_ni_skin_instance(nif, n);
	else if (type(NI_SKIN_DATA)) read_ni_skin_data(nif, n);
	else if (type(NI_SKIN_PARTITION)) 0;
	else if (type(BS_TRI_SHAPE)) 0;
	else if (type(BS_DYNAMIC_TRI_SHAPE)) 0;
	else if (type(NI_TRI_SHAPE)) 0;
	else if (type(BS_LOD_TRI_SHAPE)) 0;
	else if (type(NI_ALPHA_PROPERTY)) 0;
	else if (type(NI_TRI_SHAPE_DATA)) 0;
	else if (type(BS_EFFECT_SHADER_PROPERTY)) 0;
	else if (type(BS_EFFECT_SHADER_PROPERTY_FLOAT_CONTROLLER)) 0;
	else if (type(NI_FLOAT_INTERPOLATOR)) 0;
	else if (type(NI_FLOAT_DATA)) 0;
	else if (type(BS_LIGHTING_SHADER_PROPERTY)) 0;
	else if (type(BS_SHADER_TEXTURE_SET)) 0;
	else if (type(NI_CONTROLLER_SEQUENCE)) 0;
	else if (type(NI_TEXT_KEY_EXTRA_DATA)) 0;
	else if (type(NI_STRING_EXTRA_DATA)) 0;
	else if (type(NI_TRANSFORM_INTERPOLATOR)) 0;
	else if (type(NI_TRANSFORM_DATA)) 0;
	else if (type(BS_DECAL_PLACEMENT_VECTOR_EXTRA_DATA)) 0;
}

void read_ni_node(readparams)
{

}

void read_ni_skin_instance(readparams)
{

}

void read_ni_skin_data(readparams)
{

}