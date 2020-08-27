#include "c/c.h"

#include "nif.h"

// sorry ugly container

nmap_t nmap[1000];
int nifs = 0;

api void nif_save(void *key, nif_t *nif)
{
	nif->n = nifs;
	nmap[nifs++] = (nmap_t){key, nif};
}

api nif_t *nif_saved(void *key)
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

// lets begin

// defines macros

#define nifr nif_t *nif, int n
#define hedr nif->hdr
#define buf nif->buf
#define pos nif->pos
#define blocks nif->blocks
#define depos (buf + pos)
#define from_buf(x) *(x*)depos; pos += sizeof(x);

#define read_array(nif, c, d, e, f, g, h) sink_array(nif, d, offsetof(c, f), offsetof(c, g), sizeof(e), h)
#define read_struct(nif, c, d, e, f) sink_struct(nif, d, offsetof(c, e), offsetof(c, f))

void sink_array(nif_t *nif, unsigned char *base, int pointer, int num, int element, int is_short) {
	void **dest = base + pointer;
	int repeat = *(unsigned *)(base + num);
	if (is_short)
	repeat = *(unsigned short *)(base + num);
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
	if (i == -1)
	return NULL;
	return hedr.block_types[hedr.block_type_index[i]];
}

api ni_block_t *nif_get_block(nif_t *nif, int i) {
	if (i == -1)
	return NULL;
	return blocks[i];
}

api nif_t *nif_alloc() {
	nif_t *nif = malloc(sizeof(nif_t));
	pos = 0;
	buf = 0;
	return nif;
}

api void nif_read(nif_t *nif) {
	cassert_(buf, "buf not set");
	nif_read_header(nif);
	nif_read_blocks(nif);
}

// read header

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
	hedr.author =        nif_read_short_string(nif);
	hedr.process_script =nif_read_short_string(nif);
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

/// read blocks

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
	blocks = malloc(sizeof(ni_block_t *) * hedr.num_blocks);
	for (unsigned int i = 0; i < hedr.num_blocks; i++)
	{
	blocks[i] = NULL;
	//printf("block begin at %i %04x\n", pos, pos);
	read_block(nif, i);
	poz += hedr.block_sizes[i];
	pos = poz;
	}
}

void read_block(nif_t *nif, int n)
{
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	void *block = NULL;
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) ) block = read_ni_node(nif, n);
	else if ( ni_is_any(NI_SKIN_INSTANCE, BS_DISMEMBER_SKIN_INSTANCE, NULL) ) block = read_ni_skin_instance(nif, n);
	else if ( ni_is_type(NI_SKIN_DATA) ) block = read_ni_skin_data(nif, n);
	else if ( ni_is_type(NI_SKIN_PARTITION) ) 0;
	else if ( ni_is_type(BS_TRI_SHAPE) ) 0;
	else if ( ni_is_type(BS_DYNAMIC_TRI_SHAPE) ) 0;
	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) ) block = read_ni_tri_shape(nif, n);
	else if ( ni_is_type(NI_ALPHA_PROPERTY) ) 0;
	else if ( ni_is_type(NI_TRI_SHAPE_DATA) ) block = read_ni_tri_shape_data(nif, n);
	else if ( ni_is_type(BS_EFFECT_SHADER_PROPERTY) ) 0;
	else if ( ni_is_type(BS_EFFECT_SHADER_PROPERTY_FLOAT_CONTROLLER) ) 0;
	else if ( ni_is_type(NI_FLOAT_INTERPOLATOR) ) 0;
	else if ( ni_is_type(NI_FLOAT_DATA) ) 0;
	else if ( ni_is_type(BS_LIGHTING_SHADER_PROPERTY) ) block = read_bs_lighting_shader_property(nif, n);
	else if ( ni_is_type(BS_SHADER_TEXTURE_SET) ) block = read_bs_shader_texture_set(nif, n);
	else if ( ni_is_type(NI_CONTROLLER_SEQUENCE) ) 0;
	else if ( ni_is_type(NI_TEXT_KEY_EXTRA_DATA) ) 0;
	else if ( ni_is_type(NI_STRING_EXTRA_DATA) ) 0;
	else if ( ni_is_type(NI_TRANSFORM_INTERPOLATOR) ) 0;
	else if ( ni_is_type(NI_TRANSFORM_DATA) ) 0;
	else if ( ni_is_type(BS_DECAL_PLACEMENT_VECTOR_EXTRA_DATA) ) 0;
	blocks[n] = block;
}

ni_common_layout_t read_ni_common_layout(nifr)
{
	unsigned int size;
	ni_common_layout_t block;
	read_struct(nif, ni_common_layout_t, &block, name, extra_data_list);
	read_array(nif, ni_common_layout_t, &block, ni_ref_t, extra_data_list, num_extra_data_list, 0);
	read_struct(nif, ni_common_layout_t, &block, controller, end);
	block.name_string = nif_get_string(nif, block.name);
	return block;
}

void *read_ni_node(nifr)
{
	//printf("read ni node\n");
	ni_node_t *block = malloc(sizeof(ni_node_t));
	block->common = read_ni_common_layout(nif, n);
	read_struct(nif, ni_node_t, block, num_children, children);
	read_array(nif, ni_node_t, block, ni_ref_t, children, num_children, 0);
	read_struct(nif, ni_node_t, block, num_effects, effects);
	read_array(nif, ni_node_t, block, ni_ref_t, effects, num_effects, 0);
	return block;
}

void *read_ni_tri_shape(nifr)
{
	//printf("read ni tri shape\n");
	ni_tri_shape_t *block = malloc(sizeof(ni_node_t));
	block->common = read_ni_common_layout(nif, n);
	read_struct(nif, ni_tri_shape_t, block, data, material_data);
	pos += 9;
	read_struct(nif, ni_tri_shape_t, block, shader_property, end);
	return block;
}

void *read_ni_tri_shape_data(nifr)
{
	//printf("read ni tri shape data\n");
	ni_tri_shape_data_t *block = malloc(sizeof(ni_tri_shape_data_t));
	read_struct(nif, ni_tri_shape_data_t, block, group_id, vertices);
	if (block->has_vertices)
	read_array(nif, ni_tri_shape_data_t, block, vec_3, vertices, num_vertices, 0);
	read_struct(nif, ni_tri_shape_data_t, block, bs_vector_flags, normals);
	if (block->has_normals)
	read_array(nif, ni_tri_shape_data_t, block, vec_3, normals, num_vertices, 0);
	read_array(nif, ni_tri_shape_data_t, block, vec_3, tangents, num_vertices, 0);
	read_array(nif, ni_tri_shape_data_t, block, vec_3, bitangents, num_vertices, 0);
	read_struct(nif, ni_tri_shape_data_t, block, center, vertex_colors);
	if (block->has_vertex_colors)
	read_array(nif, ni_tri_shape_data_t, block, vec_4, vertex_colors, num_vertices, 0);
	read_array(nif, ni_tri_shape_data_t, block, vec_2, uv_sets, num_vertices, 0);
	read_struct(nif, ni_tri_shape_data_t, block, consistency_flags, triangles);
	if (block->has_triangles)
	read_array(nif, ni_tri_shape_data_t, block, ushort_3, triangles, num_triangles, 1);
	read_struct(nif, ni_tri_shape_data_t, block, num_match_groups, match_groups);
	read_array(nif, ni_tri_shape_data_t, block, ni_ref_t, match_groups, num_match_groups, 0);
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
	//printf("read bs lighting shader property\n");
	bs_lighting_shader_property_t *block = malloc(sizeof(bs_lighting_shader_property_t));
	read_struct(nif, bs_lighting_shader_property_t, block, skyrim_shader_type, extra_data_list);
	read_array(nif, bs_lighting_shader_property_t, block, ni_ref_t, extra_data_list, num_extra_data_list, 0);
	read_struct(nif, bs_lighting_shader_property_t, block, controller, end);
	block->name_string = nif_get_string(nif, block->name);
	return block;
}

void *read_bs_shader_texture_set(nifr)
{
	//printf("read bs shader texture set\n");
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

// rundown
// look at mesh.cc in dark2 for example

void visit(rd_t *, int, int);
void visit_other(rd_t *, int, int);
void visit_block(rd_t *, void *);

api rd_t *nif_alloc_rundown() {
	rd_t *rd = malloc(sizeof(rd_t));
	memset(rd, 0, sizeof(rd_t));
	rd->other = visit_other;
	rd->ni_node = rd->ni_tri_shape = rd->ni_tri_shape_data = rd->bs_lighting_shader_property = rd->bs_shader_texture_set = visit_block;
	return rd;
}

api void nif_rundown(nif_t *nif, rd_t *rd)
{
	printf("nif accept\n");
	rd->skips = malloc(sizeof(int) * hedr.num_blocks);
	memset(rd->skips, 0, sizeof(int) * hedr.num_blocks);
	rd->nif = nif;
	for (int n = 0; n < hedr.num_blocks; n++)
	{
	visit(rd, -1, n);
	}
}

void visit(rd_t *rd, int p, int c)
{
	nif_t *nif = rd->nif;
	if (-1 == c)
	return;
	if (rd->skips[c])
	return;
	rd->parent = p; rd->current = c;
	rd->skips[c] = 1;
	const char *block_type = hedr.block_types[hedr.block_type_index[c]];
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) )
	{
	ni_node_t *block = blocks[c];
	rd->ni_node(rd, block);
	for (int i = 0; i < block->num_children; i++)
	{
	int b = block->children[i];
	visit(rd, c, b);
	}
	}
	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) )
	{
	ni_tri_shape_t *block = blocks[c];
	rd->ni_tri_shape(rd, block);
	visit(rd, c, block->data);
	visit(rd, c, block->shader_property);
	visit(rd, c, block->alpha_property);
	}
	else if ( ni_is_type(NI_TRI_SHAPE_DATA) )
	{
	rd->ni_tri_shape_data(rd, blocks[c]);
	}
	else if ( ni_is_type(BS_LIGHTING_SHADER_PROPERTY) )
	{
	bs_lighting_shader_property_t *block = blocks[c];
	rd->bs_lighting_shader_property(rd, block);
	visit(rd, c, block->texture_set);
	}
	else if ( ni_is_type(BS_SHADER_TEXTURE_SET) )
	{
	rd->bs_shader_texture_set(rd, blocks[c]);
	}
}

void visit_other(rd_t *rd, int p, int c)
{

}

void visit_block(rd_t *rd, void *block)
{

}