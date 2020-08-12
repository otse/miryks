#include "c/c.h"

#include "nif.h"

#include "from_buf_helpers.h"

unsigned char *read_list(nifn, int size) {
	unsigned char *mem = malloc(size);
	memcpy(mem, buf + pos, size);
	pos += size;
	return mem;
}

void read_array(nifn, size_t element, size_t num, void *dest) {
	printf("read array %u %u\n", element, num);
	size_t size = element * num;
	if (!size) return;
	//dest = buf + pos;
	dest = malloc(size);
	memcpy(dest, buf + pos, size);
	pos += size;
}

void read_struct(nifn, int start, int stop, void *dest) {
	printf("read struct start %i stop %i\n", start, stop);
	int size = stop - start;
	if (!size) return;
	//dest = buf + pos;
	dest = malloc(size);
	memcpy(dest, buf + pos, size);
	pos += size;
}

#define as_byte(x) (unsigned char *)x

//read_as_array(nif, n, type, ni_ref_t, block, extra_data_list, num_extra_data_list);
//               a   b   c      d          e            f               g

#define read_as_array(a, b, c, d, e, f, g) read_array(a, b, sizeof(d), *(int*)((as_byte(e) + offsetof(c, g))), (as_byte(e) + offsetof(c, f)))
#define read_as_struct(a, b, c, d, e, f) read_struct(a, b, offsetof(c, e), offsetof(c, f), as_byte(d) + offsetof(c, e))

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
	read_as_array(nif, n, ni_node_t, ni_ref_t, block, children, num_children);
	block->num_effects = uint_from_buf();
	four();
	read_as_array(nif, n, ni_node_t, ni_ref_t, block, effects, num_effects);
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
	printf("ni_tri_shape_data pos %i\n", pos);
	ni_tri_shape_data_t *block = malloc(sizeof(ni_tri_shape_data_t));
	blocks[n].v = block;
	return;
	unsigned int size;
	read_as_struct(nif, n, ni_tri_shape_data_t, block, group_id, vertices);
	printf("shape data vertices pos %i\n", pos);
	read_as_array(nif, n, ni_tri_shape_data_t, vec_3, block, vertices, num_vertices);
	printf("first vec 3 %f %f %f", block->vertices[0].x, block->vertices[0].y, block->vertices[0].z);
	read_as_struct(nif, n, ni_tri_shape_data_t, block, bs_vector_flags, normals);
	read_as_array(nif, n, ni_tri_shape_data_t, vec_3, block, normals, num_vertices);
	read_as_array(nif, n, ni_tri_shape_data_t, vec_3, block, tangents, num_vertices);
	read_as_array(nif, n, ni_tri_shape_data_t, vec_3, block, bitangents, num_vertices);
	return;
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
	block->match_group = 0;
}

void read_ni_skin_instance(nifn)
{

}

void read_ni_skin_data(nifn)
{

}

void read_bs_lighting_effect_shader_property(nifn)
{
	printf("read_bs_lighting_effect_shader_property pos %i", pos);
	bs_lighting_shader_property_t *block = malloc(sizeof(bs_lighting_shader_property_t));
	blocks[n].v = block;
	read_as_struct(nif, n, bs_lighting_shader_property_t, block, skyrim_shader_type, extra_data_list);
	read_as_array(nif, n, bs_lighting_shader_property_t, ni_ref_t, block, extra_data_list, num_extra_data_list);
	read_as_struct(nif, n, bs_lighting_shader_property_t, block, controller, end);
	block->name_string = NULL;
	if (-1!=block->name)
	block->name_string = hedr.strings[block->name];
}