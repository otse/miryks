#include "c/c.h"

#include "nif.h"

#include "from_buf_helpers.h"

// in the read_as_array macro, we have to use a pointer to a pointer
// because of the as_byte(x) + offsetof arithmic funny business
void read_array(nifn, int element, int array, int num, unsigned char *block) {
	void **dest = block + array;
	int Num = *(unsigned *)(block + num);
	size_t size = element * Num;
	*dest = malloc(size);
	*dest = buf + pos;
	//memcpy(dest, buf + pos, size);
	pos += size;
}

void read_range(nifn, int start, int stop, unsigned char *block) {
	void *dest = block + start;
	int size = stop - start;
	//dest = buf + pos;
	memcpy(dest, buf + pos, size);
	pos += size;
}

char *get_hedr_string(nif_t* nif, int i) {
	char *s = NULL;
	if (i != -1)
	s = hedr.strings[i];
	return s;
}

#define read_as_array(a, b, c, d, e, f, g) read_array(a, b, sizeof(e), offsetof(c, f), offsetof(c, g), d)
#define read_as_struct(a, b, c, d, e, f) read_range(a, b, offsetof(c, e), offsetof(c, f), d)

#define read_as_array2(a, b, c, d, e, f, g) \
	read_array( \
		a, b, \
		sizeof(e), \
		*(unsigned *)((as_byte(d) + offsetof(c, g))), \
		as_byte(d) + offsetof(c, f))

void read_block(nif_t *, int);

ni_basic_layout_t read_ni_basic_layout(nifn);
void *read_ni_node(nifn);
void *read_ni_skin_instance(nifn);
void *read_ni_skin_data(nifn);
void *read_ni_tri_shape(nifn);
void *read_ni_tri_shape_data(nifn);
void *read_bs_lighting_effect_shader_property(nifn);

api void nif_read_blocks(nif_t *nif)
{
	unsigned int poz = pos;
	blocks = malloc(sizeof(ni_block_t) * hedr.num_blocks);
	for (unsigned int i = 0; i < hedr.num_blocks; i++)
	{
	blocks[i].n = i;
	blocks[i].v = 0;
	printf("block begin at %i %04x\n", pos, pos);
	read_block(nif, i);
	poz += hedr.block_sizes[i];
	pos = poz;
	}
}

#define is_type(x) 0 == strcmp(block_type, x)

void read_block(nif_t *nif, int n)
{
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	void *block = NULL;
	if (0) ;
	else if (is_type(NI_NODE)) block = read_ni_node(nif, n);
	else if (is_type(BS_LEAF_ANIM_NODE)) block = read_ni_node(nif, n);
	else if (is_type(BS_FADE_NODE)) block = read_ni_node(nif, n);
	else if (is_type(NI_SKIN_INSTANCE)) block = read_ni_skin_instance(nif, n);
	else if (is_type(BS_DISMEMBER_SKIN_INSTANCE)) block = read_ni_skin_instance(nif, n);
	else if (is_type(NI_SKIN_DATA)) block = read_ni_skin_data(nif, n);
	else if (is_type(NI_SKIN_PARTITION)) 0;
	else if (is_type(BS_TRI_SHAPE)) 0;
	else if (is_type(BS_DYNAMIC_TRI_SHAPE)) 0;
	else if (is_type(NI_TRI_SHAPE)) block = read_ni_tri_shape(nif, n);
	else if (is_type(BS_LOD_TRI_SHAPE)) 0;
	else if (is_type(NI_ALPHA_PROPERTY)) 0;
	else if (is_type(NI_TRI_SHAPE_DATA)) block = read_ni_tri_shape_data(nif, n);
	else if (is_type(BS_EFFECT_SHADER_PROPERTY)) 0;
	else if (is_type(BS_EFFECT_SHADER_PROPERTY_FLOAT_CONTROLLER)) 0;
	else if (is_type(NI_FLOAT_INTERPOLATOR)) 0;
	else if (is_type(NI_FLOAT_DATA)) 0;
	else if (is_type(BS_LIGHTING_SHADER_PROPERTY)) block = read_bs_lighting_effect_shader_property(nif, n);
	else if (is_type(BS_SHADER_TEXTURE_SET)) 0;
	else if (is_type(NI_CONTROLLER_SEQUENCE)) 0;
	else if (is_type(NI_TEXT_KEY_EXTRA_DATA)) 0;
	else if (is_type(NI_STRING_EXTRA_DATA)) 0;
	else if (is_type(NI_TRANSFORM_INTERPOLATOR)) 0;
	else if (is_type(NI_TRANSFORM_DATA)) 0;
	else if (is_type(BS_DECAL_PLACEMENT_VECTOR_EXTRA_DATA)) 0;
	blocks[n].v = block;
}

ni_basic_layout_t read_ni_basic_layout(nifn) {
	unsigned int size;
	ni_basic_layout_t block;
	read_as_struct(nif, n, ni_basic_layout_t, &block, name, extra_data_list);
	read_as_array(nif, n, ni_basic_layout_t, &block, ni_ref_t, extra_data_list, num_extra_data_list);
	read_as_struct(nif, n, ni_basic_layout_t, &block, controller, end);
	block.name_string = get_hedr_string(nif, block.name);
	return block;
}

void *read_ni_node(nifn)
{
	ni_node_t *block = malloc(sizeof(ni_node_t));
	block->basic = read_ni_basic_layout(nif, n);
	read_as_struct(nif, n, ni_node_t, block, num_children, children);
	read_as_array(nif, n, ni_node_t, block, ni_ref_t, children, num_children);
	read_as_struct(nif, n, ni_node_t, block, num_effects, effects);
	read_as_array(nif, n, ni_node_t, block, ni_ref_t, effects, num_effects);
	return block;
}

void *read_ni_tri_shape(nifn)
{
	ni_tri_shape_t *block = malloc(sizeof(ni_node_t));
	block->basic = read_ni_basic_layout(nif, n);
	read_as_struct(nif, n, ni_tri_shape_t, block, data, end);
	return block;
}

void *read_ni_tri_shape_data(nifn)
{
	ni_tri_shape_data_t *block = malloc(sizeof(ni_tri_shape_data_t));
	read_as_struct(nif, n, ni_tri_shape_data_t, block, group_id, vertices);
	read_as_array(nif, n, ni_tri_shape_data_t, block, vec_3, vertices, num_vertices);
	read_as_struct(nif, n, ni_tri_shape_data_t, block, bs_vector_flags, normals);
	read_as_array(nif, n, ni_tri_shape_data_t, block, vec_3, normals, num_vertices);
	read_as_array(nif, n, ni_tri_shape_data_t, block, vec_3, tangents, num_vertices);
	read_as_array(nif, n, ni_tri_shape_data_t, block, vec_3, bitangents, num_vertices);
	read_as_struct(nif, n, ni_tri_shape_data_t, block, center, vertex_colors);
	read_as_array(nif, n, ni_tri_shape_data_t, block, vec_4, vertex_colors, num_vertices);
	read_as_array(nif, n, ni_tri_shape_data_t, block, vec_2, uv_sets, num_vertices);
	read_as_struct(nif, n, ni_tri_shape_data_t, block, consistency_flags, triangles);
	read_as_array(nif, n, ni_tri_shape_data_t, block, ushort_3, triangles, num_triangles);
	read_as_struct(nif, n, ni_tri_shape_data_t, block, num_match_groups, match_groups);
	read_as_array(nif, n, ni_tri_shape_data_t, block, ni_ref_t, match_groups, num_match_groups);
	return block;
}

void *read_ni_skin_instance(nifn)
{

}

void *read_ni_skin_data(nifn)
{

}

void *read_bs_lighting_effect_shader_property(nifn)
{
	bs_lighting_shader_property_t *block = malloc(sizeof(bs_lighting_shader_property_t));
	read_as_struct(nif, n, bs_lighting_shader_property_t, block, skyrim_shader_type, extra_data_list);
	read_as_array(nif, n, bs_lighting_shader_property_t, block, ni_ref_t, extra_data_list, num_extra_data_list);
	read_as_struct(nif, n, bs_lighting_shader_property_t, block, controller, end);
	block->name_string = get_hedr_string(nif, block->name);
	return block;
}