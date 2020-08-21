#include "c/c.h"

#include "nif.h"

nmap_t nmap[1000];
int nifs = 0;

//const char *path = "path not set";

api nif_t *nif_alloc() {
	nif_t *bucket = malloc(sizeof(nif_t));
	bucket->pos = 0;
	bucket->buf = 0;
	return bucket;
}

api void nif_add(void *key, nif_t *nif)
{
	nif->n = nifs;
	nmap[nifs++] = (nmap_t){key, nif};
}

// todo generalize this container
api nif_t *nif_get_stored(void *key)
{
	for (int i = 0; i < nifs; i++)
	{
	if (nmap[i].key == key)
	{
	return nmap[i].value;
	}
	}
	return 0;
}

api void nif_make(void *key, nif_t *nif)
{
	cassert_(nif->buf, "nif->buf NULL");
	nif_read_header(nif);
	nif_read_blocks(nif);
	nif_add(key, nif);
}

// defines macros:

#define nifr nif_t *nif, int n
#define hedr nif->hdr
#define buf nif->buf
#define pos nif->pos
#define blocks nif->blocks
#define depos (buf + pos)
#define from_buf(x) *(x*)depos; pos += sizeof(x);

#define read_array(nif, c, d, e, f, g) sink_array(nif, d, offsetof(c, f), offsetof(c, g), sizeof(e))
#define read_struct(nif, c, d, e, f) sink_struct(nif, d, offsetof(c, e), offsetof(c, f))

void sink_array(nif_t *nif, unsigned char *base, int pointer, int num, int element) {
	void **dest = base + pointer;
	int repeat = *(unsigned *)(base + num);
	int size = element * repeat;
	*dest = malloc(size);
	*dest = depos;
	pos += size;
}

void sink_struct(nif_t *nif, unsigned char *base, int start, int stop) {
	void *dest = base + start;
	int size = stop - start;
	memcpy(dest, depos, size);
	pos += size;
}

// getters:

api char *nif_get_string(nif_t *nif, int i) {
	if (i == -1)
	return NULL;
	return hedr.strings[i];
}

api char *nif_get_block_type(nif_t *nif, int i) {
	return hedr.block_types[i];
}

api ni_block_t *nif_get_block(nif_t *nif, int i) {
	if (i == -1)
	return NULL;
	return &blocks[i];
}

// read header:

void read_header_string(nif_t *);
void read_some_stuff(nif_t *);
void read_block_types(nif_t *);
void read_block_type_index(nif_t *);
void read_block_sizes(nif_t *);
void read_strings(nif_t *);
void read_groups(nif_t *);

char *nif_read_short_string(nif_t *nif)
{
	char len = from_buf(char);
	char *string = malloc(sizeof(char) * len);
	strncpy(string, depos, len);
	pos += len;
	return string;
}

char *nif_read_sized_string(nif_t *nif)
{
	int len = from_buf(int);
	printf("sized string len %i", len);
	char *string = malloc(sizeof(char) * len + 1);
	strncpy(string, depos, len);
	string[len] = '\0';
	pos += len;
	return string;
}

api void nif_read_header(nif_t *nif)
{
	read_header_string(nif);
	hedr.unknown_1 = from_buf(int);
	// later on blocks use aggressive macros to read
	read_some_stuff(nif);
	read_block_types(nif);
	read_block_type_index(nif);
	read_block_sizes(nif);
	read_strings(nif);
	read_groups(nif);
	hedr.end = pos;
}

void read_header_string(nif_t *nif)
{
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(buf, '\n') - buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, buf, n);
	string[n - 1] = '\0';
	hedr.header_string = string;
	hedr.version = string + 30;
	pos += n;
}

void read_some_stuff(nif_t *nif)
{
	hedr.endian_type = from_buf(unsigned char);
	hedr.user_value =  from_buf(unsigned int);
	hedr.num_blocks =  from_buf(unsigned int);
	hedr.user_value_2 =from_buf(unsigned int);
	hedr.author = nif_read_short_string(nif);
	hedr.process_script = nif_read_short_string(nif);
	hedr.export_script = nif_read_short_string(nif);
	hedr.num_block_types = from_buf(unsigned short);
}

void read_block_types(nif_t *nif)
{
	int n = hedr.num_block_types;
	hedr.block_types = malloc(sizeof(char *) * n);
	for (int i = 0; i < n; i++)
	{
	hedr.block_types[i] = nif_read_sized_string(nif);
	}
}

void read_block_type_index(nif_t *nif)
{
	int size = sizeof(unsigned short) * hedr.num_blocks;
	hedr.block_type_index = malloc(size);
	memcpy(hedr.block_type_index, depos, size);
	pos += size;
}

void read_block_sizes(nif_t *nif)
{
	int size = sizeof(unsigned int) * hedr.num_blocks;
	hedr.block_sizes = malloc(size);
	memcpy(hedr.block_sizes, depos, size);
	pos += size;
}

void read_strings(nif_t *nif)
{
	hedr.num_strings = from_buf(unsigned int);
	hedr.max_string_length = from_buf(unsigned int);
	int n = hedr.num_strings;
	hedr.strings = malloc(sizeof(char *) * n);
	for (int i = 0; i < n; i++)
	{
	hedr.strings[i] = nif_read_sized_string(nif);
	}
}

void read_groups(nif_t *nif)
{
	hedr.num_groups = from_buf(unsigned int);
}

/// read blocks:

void read_block(nif_t *, int);

ni_common_layout_t read_ni_common_layout(nifr);
void *read_ni_node(nifr);
void *read_ni_skin_instance(nifr);
void *read_ni_skin_data(nifr);
void *read_ni_tri_shape(nifr);
void *read_ni_tri_shape_data(nifr);
void *read_bs_lighting_shader_property(nifr);
void *read_bs_shader_texture_set(nifr);

api void nif_read_blocks(nif_t *nif)
{
	unsigned int poz = pos;
	blocks = malloc(sizeof(ni_block_t) * hedr.num_blocks);
	for (unsigned int i = 0; i < hedr.num_blocks; i++)
	{
	blocks[i]=(ni_block_t){i,NULL};
	//printf("block begin at %i %04x\n", pos, pos);
	read_block(nif, i);
	poz += hedr.block_sizes[i];
	pos = poz;
	}
}

#define is_type(x) 0 == strcmp(block_type, x)
#define is_any_type(x, y, z) is_type(x) || (y ? is_type(y) : 0) || (z ? is_type(z) : 0)

void read_block(nif_t *nif, int n)
{
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	void *block = NULL;
	if (0) ;
	else if ( is_any_type(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) ) block = read_ni_node(nif, n);
	else if ( is_any_type(NI_SKIN_INSTANCE, BS_DISMEMBER_SKIN_INSTANCE, NULL) ) block = read_ni_skin_instance(nif, n);
	else if ( is_type(NI_SKIN_DATA) ) block = read_ni_skin_data(nif, n);
	else if ( is_type(NI_SKIN_PARTITION) ) 0;
	else if ( is_type(BS_TRI_SHAPE) ) 0;
	else if ( is_type(BS_DYNAMIC_TRI_SHAPE) ) 0;
	else if ( is_any_type(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) ) block = read_ni_tri_shape(nif, n);
	else if ( is_type(NI_ALPHA_PROPERTY) ) 0;
	else if ( is_type(NI_TRI_SHAPE_DATA) ) block = read_ni_tri_shape_data(nif, n);
	else if ( is_type(BS_EFFECT_SHADER_PROPERTY) ) 0;
	else if ( is_type(BS_EFFECT_SHADER_PROPERTY_FLOAT_CONTROLLER) ) 0;
	else if ( is_type(NI_FLOAT_INTERPOLATOR) ) 0;
	else if ( is_type(NI_FLOAT_DATA) ) 0;
	else if ( is_type(BS_LIGHTING_SHADER_PROPERTY) ) block = read_bs_lighting_shader_property(nif, n);
	else if ( is_type(BS_SHADER_TEXTURE_SET) ) block = read_bs_shader_texture_set(nif, n);
	else if ( is_type(NI_CONTROLLER_SEQUENCE) ) 0;
	else if ( is_type(NI_TEXT_KEY_EXTRA_DATA) ) 0;
	else if ( is_type(NI_STRING_EXTRA_DATA) ) 0;
	else if ( is_type(NI_TRANSFORM_INTERPOLATOR) ) 0;
	else if ( is_type(NI_TRANSFORM_DATA) ) 0;
	else if ( is_type(BS_DECAL_PLACEMENT_VECTOR_EXTRA_DATA) ) 0;
	blocks[n].v = block;
}

ni_common_layout_t read_ni_common_layout(nifr)
{
	unsigned int size;
	ni_common_layout_t block;
	read_struct(nif, ni_common_layout_t, &block, name, extra_data_list);
	read_array(nif, ni_common_layout_t, &block, ni_ref_t, extra_data_list, num_extra_data_list);
	read_struct(nif, ni_common_layout_t, &block, controller, end);
	block.name_string = nif_get_string(nif, block.name);
	return block;
}

void *read_ni_node(nifr)
{
	printf("read ni node\n");
	ni_node_t *block = malloc(sizeof(ni_node_t));
	block->common = read_ni_common_layout(nif, n);
	read_struct(nif, ni_node_t, block, num_children, children);
	read_array(nif, ni_node_t, block, ni_ref_t, children, num_children);
	read_struct(nif, ni_node_t, block, num_effects, effects);
	read_array(nif, ni_node_t, block, ni_ref_t, effects, num_effects);
	return block;
}

void *read_ni_tri_shape(nifr)
{
	printf("read ni tri shape\n");
	ni_tri_shape_t *block = malloc(sizeof(ni_node_t));
	block->common = read_ni_common_layout(nif, n);
	read_struct(nif, ni_tri_shape_t, block, data, material_data);
	pos += 9;
	read_struct(nif, ni_tri_shape_t, block, shader_property, end);
	return block;
}

void *read_ni_tri_shape_data(nifr)
{
	printf("read ni tri shape data\n");
	ni_tri_shape_data_t *block = malloc(sizeof(ni_tri_shape_data_t));
	read_struct(nif, ni_tri_shape_data_t, block, group_id, vertices);
	read_array(nif, ni_tri_shape_data_t, block, vec_3, vertices, num_vertices);
	read_struct(nif, ni_tri_shape_data_t, block, bs_vector_flags, normals);
	read_array(nif, ni_tri_shape_data_t, block, vec_3, normals, num_vertices);
	read_array(nif, ni_tri_shape_data_t, block, vec_3, tangents, num_vertices);
	read_array(nif, ni_tri_shape_data_t, block, vec_3, bitangents, num_vertices);
	read_struct(nif, ni_tri_shape_data_t, block, center, vertex_colors);
	read_array(nif, ni_tri_shape_data_t, block, vec_4, vertex_colors, num_vertices);
	read_array(nif, ni_tri_shape_data_t, block, vec_2, uv_sets, num_vertices);
	read_struct(nif, ni_tri_shape_data_t, block, consistency_flags, triangles);
	read_array(nif, ni_tri_shape_data_t, block, ushort_3, triangles, num_triangles);
	read_struct(nif, ni_tri_shape_data_t, block, num_match_groups, match_groups);
	read_array(nif, ni_tri_shape_data_t, block, ni_ref_t, match_groups, num_match_groups);
	return block;
}

void *read_ni_skin_instance(nifr)
{
	//read_block_save(nif, n, block);

}

void *read_ni_skin_data(nifr)
{
	//read_block_save(nif, n, block);
}

void *read_bs_lighting_shader_property(nifr)
{
	printf("read bs lighting shader property\n");

	bs_lighting_shader_property_t *block = malloc(sizeof(bs_lighting_shader_property_t));
	read_struct(nif, bs_lighting_shader_property_t, block, skyrim_shader_type, extra_data_list);
	read_array(nif, bs_lighting_shader_property_t, block, ni_ref_t, extra_data_list, num_extra_data_list);
	read_struct(nif, bs_lighting_shader_property_t, block, controller, end);
	block->name_string = nif_get_string(nif, block->name);
	return block;
}

void *read_bs_shader_texture_set(nifr)
{
	printf("read bs shader texture set\n");

	bs_shader_texture_set_t *block = malloc(sizeof(bs_shader_texture_set_t));
	read_struct(nif, bs_shader_texture_set_t, block, num_textures, textures);
	block->textures = malloc(sizeof(char *) * block->num_textures);
	memset(block->textures, '\0', block->num_textures);
	int l = block->num_textures;
	for (unsigned i = 0; i < l; i++)
	{
	block->textures[i] = nif_read_sized_string(nif);
	}
	return block;
}

// meshify

#define nifm nif_t *nif, ni_block_t *block

void meshify_ni_node(nifm)
{

}

void nif_loop(nif_t *nif)
{
	group_t group={0,0,0,0};
	for (int n = 0; n < hedr.num_blocks; n++)
	{
	ni_block_t *block = nif_get_block(nif, n);
	const char *block_type = nif_get_block_type(nif, n);
	if (0) ;
	else if ( is_any_type(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) )
	meshify_ni_node(nif, block);
	}
	/*
	gr
	*/
}