#include "c/c.h"

#include "nif.h"

#include "from_buf_helpers.h"

void read_block(nif_t *, int);

void read_ni_node(nifn);
void read_ni_skin_instance(nifn);
void read_ni_skin_data(nifn);

api void nif_read_blocks(nif_t *nif)
{
	blocks = malloc(sizeof(ni_block_t) * hedr.num_blocks);
	for (int i = 0; i < hedr.num_blocks; i++)
	{
	blocks[i].n = i;
	blocks[i].v = NULL;
	printf("block begin at %i", pos);
	read_block(nif, i);
	}
}

#define is_type(x) 0 == strcmp(block_type, x)

void read_block(nif_t *nif, int n)
{
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	if (0) ;
	else if (is_type(NI_NODE)) read_ni_node(nif, n);
	else if (is_type(BS_LEAF_ANIM_NODE)) read_ni_node(nif, n);
	else if (is_type(BS_FADE_NODE)) read_ni_node(nif, n);
	else if (is_type(NI_SKIN_INSTANCE)) read_ni_skin_instance(nif, n);
	else if (is_type(BS_DISMEMBER_SKIN_INSTANCE)) read_ni_skin_instance(nif, n);
	else if (is_type(NI_SKIN_DATA)) read_ni_skin_data(nif, n);
	else if (is_type(NI_SKIN_PARTITION)) 0;
	else if (is_type(BS_TRI_SHAPE)) 0;
	else if (is_type(BS_DYNAMIC_TRI_SHAPE)) 0;
	else if (is_type(NI_TRI_SHAPE)) 0;
	else if (is_type(BS_LOD_TRI_SHAPE)) 0;
	else if (is_type(NI_ALPHA_PROPERTY)) 0;
	else if (is_type(NI_TRI_SHAPE_DATA)) 0;
	else if (is_type(BS_EFFECT_SHADER_PROPERTY)) 0;
	else if (is_type(BS_EFFECT_SHADER_PROPERTY_FLOAT_CONTROLLER)) 0;
	else if (is_type(NI_FLOAT_INTERPOLATOR)) 0;
	else if (is_type(NI_FLOAT_DATA)) 0;
	else if (is_type(BS_LIGHTING_SHADER_PROPERTY)) 0;
	else if (is_type(BS_SHADER_TEXTURE_SET)) 0;
	else if (is_type(NI_CONTROLLER_SEQUENCE)) 0;
	else if (is_type(NI_TEXT_KEY_EXTRA_DATA)) 0;
	else if (is_type(NI_STRING_EXTRA_DATA)) 0;
	else if (is_type(NI_TRANSFORM_INTERPOLATOR)) 0;
	else if (is_type(NI_TRANSFORM_DATA)) 0;
	else if (is_type(BS_DECAL_PLACEMENT_VECTOR_EXTRA_DATA)) 0;
}

void read_ni_node(nifn)
{
	unsigned int size;
	ni_node_t *ni_node = malloc(sizeof(ni_node_t));
	blocks[n].v = ni_node;
	ni_node->name = from_buf();
	ni_node->name_string = NULL;
	if (-1 != ni_node->name)
	ni_node->name_string = hedr.strings[ni_node->name];
	four();
	ni_node->num_extra_data_list = from_buf();
	four();
	size = sizeof(ni_ref_t) * ni_node->num_extra_data_list;
	ni_node->extra_data_list = malloc(size);
	memcpy(ni_node->extra_data_list, buf + pos, size);
	pos += size;
	ni_node->controller = from_buf();
	printf("\ncontroller: %i\n", ni_node->controller);
	four();
	ni_node->flags = from_buf();
	four();
	//*ni_node->translation = from_buf();
	memcpy(ni_node->translation, buf + pos, four() * 3);
	four() * 3;
	*ni_node->rotation = from_buf();
	four() * 9;
	ni_node->scale = from_buf();
	four();
	ni_node->collision_object = from_buf();
	four();
	ni_node->num_children = from_buf();
	four();
	size = sizeof(ni_ref_t) * ni_node->num_children;
	ni_node->children = malloc(size);
	memcpy(ni_node->children, buf + pos, size);
	pos += size;
}

void read_ni_skin_instance(nifn)
{

}

void read_ni_skin_data(nifn)
{

}