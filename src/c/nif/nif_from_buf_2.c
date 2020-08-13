#include "c/c.h"

#include "nif.h"

#include "from_buf_helpers.h"

unsigned char *read_list(nifn, int size) {
	unsigned char *mem = malloc(size);
	memcpy(mem, buf + pos, size);
	pos += size;
	return mem;
}

// todo, point struct pointer to buf + pos instead of a memcpy

void read_array(nifn, int element, int num, void **dest) {
	size_t size = element * num;
	*dest = malloc(size);
	*dest = buf + pos;
	//memcpy(dest, buf + pos, size);
	pos += size;
}

void read_range(nifn, int start, int stop, void *dest) {
	int size = stop - start;
	//dest = buf + pos;
	memcpy(dest, buf + pos, size);
	pos += size;
}

#define as_byte(x) (unsigned char *)x
//read_as_array(nif, n, type, block, ni_ref_t, extra_data_list, num_extra_data_list)
//               a   b   c      d        e             f                 g
#define read_as_array(a, b, c, d, e, f, g) read_array(a, b, sizeof(e), *(unsigned *)((as_byte(d) + offsetof(c, g))),  as_byte(d) + offsetof(c, f))
#define read_as_struct(a, b, c, d, e, f) read_range(a, b, offsetof(c, e), offsetof(c, f), as_byte(d) + offsetof(c, e))

void read_block(nif_t *, int);

ni_basic_layout_t read_ni_basic_layout(nifn);
void read_ni_node(nifn);
void read_ni_skin_instance(nifn);
void read_ni_skin_data(nifn);
void read_ni_tri_shape(nifn);
void read_ni_tri_shape_data(nifn);
void read_bs_lighting_effect_shader_property(nifn);

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
	else if (is_type(NI_TRI_SHAPE)) read_ni_tri_shape(nif, n);
	else if (is_type(BS_LOD_TRI_SHAPE)) 0;
	else if (is_type(NI_ALPHA_PROPERTY)) 0;
	else if (is_type(NI_TRI_SHAPE_DATA)) read_ni_tri_shape_data(nif, n);
	else if (is_type(BS_EFFECT_SHADER_PROPERTY)) 0;
	else if (is_type(BS_EFFECT_SHADER_PROPERTY_FLOAT_CONTROLLER)) 0;
	else if (is_type(NI_FLOAT_INTERPOLATOR)) 0;
	else if (is_type(NI_FLOAT_DATA)) 0;
	else if (is_type(BS_LIGHTING_SHADER_PROPERTY)) read_bs_lighting_effect_shader_property(nif, n);
	else if (is_type(BS_SHADER_TEXTURE_SET)) 0;
	else if (is_type(NI_CONTROLLER_SEQUENCE)) 0;
	else if (is_type(NI_TEXT_KEY_EXTRA_DATA)) 0;
	else if (is_type(NI_STRING_EXTRA_DATA)) 0;
	else if (is_type(NI_TRANSFORM_INTERPOLATOR)) 0;
	else if (is_type(NI_TRANSFORM_DATA)) 0;
	else if (is_type(BS_DECAL_PLACEMENT_VECTOR_EXTRA_DATA)) 0;
}

ni_basic_layout_t read_ni_basic_layout(nifn) {
	unsigned int size;
	ni_basic_layout_t block;
	block.name = int_from_buf();
	four();
	block.num_extra_data_list = uint_from_buf();
	four();
	size = sizeof(ni_ref_t) * block.num_extra_data_list;
	block.extra_data_list = read_list(nif, n, size);
	block.controller = int_from_buf();
	four();
	block.flags = uint_from_buf();
	four();
	block.translation = vec_3_from_buf();
	four() * 3;
	block.rotation = mat_3_from_buf();
	four() * 9;
	block.scale = float_from_buf();
	four();
	block.collision_object = int_from_buf();
	printf("col obj: %i", block.collision_object);
	four();
	//
	block.name_string = NULL;
	if (-1 != block.name)
	block.name_string = hedr.strings[block.name];
	return block;
}

void read_ni_node(nifn)
{
	ni_node_t *block = malloc(sizeof(ni_node_t));
	blocks[n].v = block;
	block->basic = read_ni_basic_layout(nif, n);
	block->num_children = uint_from_buf();
	four();
	read_as_array(nif, n, ni_node_t, block, ni_ref_t, children, num_children);
	block->num_effects = uint_from_buf();
	four();
	read_as_array(nif, n, ni_node_t, block, ni_ref_t, effects, num_effects);
}

void read_ni_tri_shape(nifn)
{
	ni_tri_shape_t *block = malloc(sizeof(ni_node_t));
	blocks[n].v = block;
	block->basic = read_ni_basic_layout(nif, n);
	block->data = int_from_buf();
	four();
	block->skin_instance = int_from_buf();
	four();
	// material data
	pos += 9;
	block->shader_property = int_from_buf();
	four();
	block->alpha_property = int_from_buf();
	four();
}

void read_ni_tri_shape_data(nifn)
{
	printf("ni tri shape data\n");
	ni_tri_shape_data_t *block = malloc(sizeof(ni_tri_shape_data_t));
	blocks[n].v = block;
	unsigned size;
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
	//read_as_array(nif, n, ni_tri_shape_data_t, block, ushort_3, triangles, num_triangles);
	//read_as_struct(nif, n, ni_tri_shape_data_t, block, num_match_groups, match_groups);
	//read_as_array(nif, n, ni_tri_shape_data_t, block, ni_ref_t, match_groups, num_match_groups);
	return;
	block->group_id = int_from_buf();
	four();
	block->num_vertices = ushort_from_buf();
	two();
	block->keep_flags = byte_from_buf();
	printf("offsetof %i", offsetof(ni_tri_shape_data_t, keep_flags));
	one();
	block->compress_flags = byte_from_buf();
	one();
	block->has_vertices = byte_from_buf();
	one();
	size = sizeof(vec_3) * block->num_vertices;
	block->vertices = read_list(nif, n, size);
	block->bs_vector_flags = ushort_from_buf();
	two();
	block->material_crc = uint_from_buf();
	four();
	block->has_normals = byte_from_buf();
	one();
	size = sizeof(vec_3) * block->num_vertices;
	block->normals = read_list(nif, n, size);
	size = sizeof(vec_3) * block->num_vertices;
	block->tangents = read_list(nif, n, size);
	size = sizeof(vec_3) * block->num_vertices;
	block->bitangents = read_list(nif, n, size);
	block->center = vec_3_from_buf();
	four() * 3;
	block->radius = float_from_buf();
	four();
	block->has_vertex_colors = byte_from_buf();
	one();
	size = sizeof(vec_4) * block->num_vertices;
	block->vertex_colors = read_list(nif, n, size);
	size = sizeof(vec_2) * block->num_vertices;
	block->uv_sets = read_list(nif, n, size);
	block->consistency_flags = ushort_from_buf();
	two();
	block->additional_data = ushort_from_buf();
	four();
	block->num_triangles = ushort_from_buf();
	two();
	block->num_triangle_points = uint_from_buf();
	four();
	block->has_triangles = byte_from_buf();
	one();
	size = sizeof(ushort_3) * block->num_triangles;
	block->triangles = read_list(nif, n, size);
	block->num_match_groups = ushort_from_buf();
	two();
	block->match_groups = 0;
}

void read_ni_skin_instance(nifn)
{

}

void read_ni_skin_data(nifn)
{

}

void read_bs_lighting_effect_shader_property(nifn)
{
	bs_lighting_shader_property_t *block = malloc(sizeof(bs_lighting_shader_property_t));
	blocks[n].v = block;
	read_as_struct(nif, n, bs_lighting_shader_property_t, block, skyrim_shader_type, extra_data_list);
	read_as_array(nif, n, bs_lighting_shader_property_t, block, ni_ref_t, extra_data_list, num_extra_data_list);
	read_as_struct(nif, n, bs_lighting_shader_property_t, block, controller, end);
	block->name_string = NULL;
	if (-1!=block->name)
	block->name_string = hedr.strings[block->name];
}