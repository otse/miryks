#include "putc.h"

#include "nif.h"

// successed by nifp.c

struct nmap_t nmap[1000];
int nifs = 0;

api void nif_save(void *key, struct nif *nif)
{
	nif->n = nifs;
	nmap[nifs++] = (struct nmap_t){key, nif};
}

api struct nif *nif_saved(void *key)
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

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks

#define Depos  Buf + Pos
#define FromBuf(x) *(x*)(Depos); Pos += sizeof(x);

#define ReadArray(nif, c, d, e, f, g, h) sink_array(nif, d, offsetof(struct c, f), offsetof(struct c, g), sizeof(e), h)
#define ReadStruct(nif, c, d, e, f) sink_struct(nif, d, offsetof(struct c, e), offsetof(struct c, f))

void sink_array(struct nif *nif, unsigned char *base, int offset, int num, int element, int is_short) {
	void **dest = base + offset;
	int repeat;
	if (!is_short)
	repeat = *(unsigned int*)(base + num);
	else
	repeat = *(unsigned short *)(base + num);
	int size = element * repeat;
	if (size == 0)
	return;
	*dest = malloc(size);
	memcpy(*dest, Depos, size);
	//*dest = Depos;
	Pos += size;
}

// sink values
void sink_struct(struct nif *nif, unsigned char *base, int start, int stop) {
	void *dest = base + start;
	int size = stop - start;
	memcpy(dest, Depos, size);
	Pos += size;
}

// sink values as pointers
// impossible. struct-sinking relys on the memory-layout for correctness
// pointers to data would always be 8*8bit
// workaround = convert the end field to size, which points .. at sizes?
// tackled in nifp.c
void sink_struct_2(struct nif *nif, unsigned char *base, int start, int stop) {
	int size = stop - start;
	unsigned char *dest = base + start;
	int num_fields = size / sizeof(void *);
	for (int i = 0; i < num_fields; i++)
	*(dest + i * sizeof(void *)) = start + i + sizeof(void *);
	//memcpy(dest, Depos, size);
	Pos += size;
}

api char *nif_get_string(struct nif *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr.strings[i];
}

api char *nif_get_block_type(struct nif *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr.block_types[Hedr.block_type_index[i]];
}

api void *nif_get_block(struct nif *nif, int i) {
	if (i == -1)
	return NULL;
	return Blocks[i];
}

api struct nif *nif_alloc() {
	struct nif *nif = malloc(sizeof(struct nif));
	Pos = 0;
	Buf = 0;
	return nif;
}

api void nif_read(struct nif *nif) {
	cassert(Buf, "nif_read Buf not set");
	nif_read_header(nif);
	nif_read_blocks(nif);
}

void hedr_read_header_string(struct nif *);
void hedr_read_vars(struct nif *);
void hedr_read_block_types(struct nif *);
void hedr_read_block_type_index(struct nif *);
void hedr_read_block_sizes(struct nif *);
void hedr_read_strings(struct nif *);
void hedr_read_groups(struct nif *);

char *nif_read_short_string(struct nif *nif) {
	char len = FromBuf(char);
	char *string = malloc(sizeof(char) * len);
	strncpy(string, Depos, len);
	Pos += len;
	return string;
}

char *nif_read_sized_string(struct nif *nif) {
	int len = FromBuf(int);
	char *string = malloc(sizeof(char) * len + 1);
	strncpy(string, Depos, len);
	string[len] = '\0';
	Pos += len;
	return string;
}

api void nif_read_header(struct nif *nif) {
	hedr_read_header_string(nif);
	Hedr.unknown_1 = FromBuf(int);
	// later on blocks use aggressive macros to read
	hedr_read_vars(nif);
	hedr_read_block_types(nif);
	hedr_read_block_type_index(nif);
	hedr_read_block_sizes(nif);
	hedr_read_strings(nif);
	hedr_read_groups(nif);
	Hedr.end = Pos;
}

void hedr_read_header_string(struct nif *nif) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(Buf, '\n') - Buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, Buf, n);
	string[n - 1] = '\0';
	Hedr.header_string = string;
	Hedr.version = string + 30;
	Pos += n;
}

void hedr_read_vars(struct nif *nif) {
	Hedr.endian_type = FromBuf(unsigned char);
	Hedr.user_value =  FromBuf(unsigned int);
	Hedr.num_blocks =  FromBuf(unsigned int);
	Hedr.user_value_2 =FromBuf(unsigned int);
	Hedr.author =        nif_read_short_string(nif);
	Hedr.process_script =nif_read_short_string(nif);
	Hedr.export_script = nif_read_short_string(nif);
	Hedr.num_block_types = FromBuf(unsigned short);
}

void hedr_read_block_types(struct nif *nif) {
	int n = Hedr.num_block_types;
	Hedr.block_types = malloc(sizeof(char *) * n);
	for (int i = 0; i < n; i++)
	{
	Hedr.block_types[i] = nif_read_sized_string(nif);
	}
}

void hedr_read_block_type_index(struct nif *nif) {
	int size = sizeof(unsigned short) * Hedr.num_blocks;
	Hedr.block_type_index = malloc(size);
	memcpy(Hedr.block_type_index, Depos, size);
	Pos += size;
}

void hedr_read_block_sizes(struct nif *nif) {
	int size = sizeof(unsigned int) * Hedr.num_blocks;
	Hedr.block_sizes = malloc(size);
	memcpy(Hedr.block_sizes, Depos, size);
	Pos += size;
}

void hedr_read_strings(struct nif *nif) {
	Hedr.num_strings = FromBuf(unsigned int);
	Hedr.max_string_length = FromBuf(unsigned int);
	int n = Hedr.num_strings;
	Hedr.strings = malloc(sizeof(char *) * n);
	for (int i = 0; i < n; i++)
	{
	Hedr.strings[i] = nif_read_sized_string(nif);
	}
}

void hedr_read_groups(struct nif *nif) {
	Hedr.num_groups = FromBuf(unsigned int);
}

#define nifr struct nif *nif, int n

void read_block(struct nif *, int);

struct ni_common_layout read_ni_common_layout(nifr);
void *read_ni_node(nifr);
void *read_ni_skin_instance(nifr);
void *read_ni_skin_data(nifr);
void *read_ni_tri_shape(nifr);
void *read_ni_tri_shape_data(nifr);
void *read_bs_lighting_shader_property(nifr);
void *read_bs_shader_texture_set(nifr);
void *read_ni_alpha_property(nifr);

api void nif_read_blocks(struct nif *nif)
{
	unsigned int pos = Pos;
	Blocks = malloc(sizeof(void *) * Hedr.num_blocks);
	for (unsigned int i = 0; i < Hedr.num_blocks; i++)
	{
	Blocks[i] = NULL;
	//printf("block begin at %i %04x\n", Pos, Pos);
	read_block(nif, i);
	pos += Hedr.block_sizes[i];
	Pos = pos;
	}
}

void read_block(struct nif *nif, int n)
{
	const char *block_type = Hedr.block_types[Hedr.block_type_index[n]];
	void *block = NULL;
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) ) block = read_ni_node(nif, n);
	else if ( ni_is_any(NI_SKIN_INSTANCE, BS_DISMEMBER_SKIN_INSTANCE, NULL) ) block = read_ni_skin_instance(nif, n);
	else if ( ni_is_type(NI_SKIN_DATA) ) block = read_ni_skin_data(nif, n);
	else if ( ni_is_type(NI_SKIN_PARTITION) ) 0;
	else if ( ni_is_type(BS_TRI_SHAPE) ) 0;
	else if ( ni_is_type(BS_DYNAMIC_TRI_SHAPE) ) 0;
	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) ) block = read_ni_tri_shape(nif, n);
	else if ( ni_is_type(NI_ALPHA_PROPERTY) ) block = read_ni_alpha_property(nif, n);
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
	Blocks[n] = block;
}

struct ni_common_layout read_ni_common_layout(nifr)
{
	unsigned int size;
	struct ni_common_layout block;
	ReadStruct(nif, ni_common_layout, &block, name, extra_data_list);
	ReadArray(nif, ni_common_layout, &block, ni_ref, extra_data_list, num_extra_data_list, 0);
	ReadStruct(nif, ni_common_layout, &block, controller, end);
	block.name_string = nif_get_string(nif, block.name);
	return block;
}

void *read_ni_node(nifr)
{
	//printf("read ni node\n");
	struct ni_node *block = malloc(sizeof(struct ni_node));
	block->common = read_ni_common_layout(nif, n);
	ReadStruct(nif, ni_node, block, num_children, children);
	ReadArray(nif, ni_node, block, ni_ref, children, num_children, 0);
	ReadStruct(nif, ni_node, block, num_effects, effects);
	ReadArray(nif, ni_node, block, ni_ref, effects, num_effects, 0);
	return block;
}

void *read_ni_tri_shape(nifr)
{
	//printf("read ni tri shape\n");
	struct ni_tri_shape *block = malloc(sizeof(struct ni_node));
	block->common = read_ni_common_layout(nif, n);
	ReadStruct(nif, ni_tri_shape, block, data, material_data);
	Pos += 9;
	ReadStruct(nif, ni_tri_shape, block, shader_property, end);
	return block;
}

void *read_ni_tri_shape_data(nifr)
{
	//printf("read ni tri shape data\n");
	struct ni_tri_shape_data *block = malloc(sizeof(struct ni_tri_shape_data));
	ReadStruct(nif, ni_tri_shape_data, block, group_id, vertices);
	if (block->has_vertices)
	ReadArray(nif, ni_tri_shape_data, block, struct vec_3, vertices, num_vertices, 0);
	ReadStruct(nif, ni_tri_shape_data, block, bs_vector_flags, normals);
	if (block->has_normals)
	ReadArray(nif, ni_tri_shape_data, block, struct vec_3, normals, num_vertices, 0);
	if (block->bs_vector_flags & 0x00001000)
	{
	ReadArray(nif, ni_tri_shape_data, block, struct vec_3, tangents, num_vertices, 0);
	ReadArray(nif, ni_tri_shape_data, block, struct vec_3, bitangents, num_vertices, 0);
	}
	ReadStruct(nif, ni_tri_shape_data, block, center, vertex_colors);
	if (block->has_vertex_colors)
	ReadArray(nif, ni_tri_shape_data, block, struct vec_4, vertex_colors, num_vertices, 0);
	if (block->bs_vector_flags & 0x00000001)
	ReadArray(nif, ni_tri_shape_data, block, struct vec_2, uv_sets, num_vertices, 0);
	ReadStruct(nif, ni_tri_shape_data, block, consistency_flags, triangles);
	if (block->has_triangles)
	ReadArray(nif, ni_tri_shape_data, block, struct ushort_3, triangles, num_triangles, 1);
	ReadStruct(nif, ni_tri_shape_data, block, num_match_groups, match_groups);
	ReadArray(nif, ni_tri_shape_data, block, ni_ref, match_groups, num_match_groups, 1);
	return block;
}

void *read_ni_skin_instance(nifr)
{
	//read_block_save(nif, n, block);
	return NULL;
}

void *read_ni_skin_data(nifr)
{
	//read_block_save(nif, n, block);
	return NULL;
}

void *read_bs_lighting_shader_property(nifr)
{
	//printf("read bs lighting shader property\n");
	struct bs_lighting_shader_property *block = malloc(sizeof(struct bs_lighting_shader_property));
	ReadStruct(nif, bs_lighting_shader_property, block, skyrim_shader_type, extra_data_list);
	ReadArray(nif, bs_lighting_shader_property, block, ni_ref, extra_data_list, num_extra_data_list, 0);
	ReadStruct(nif, bs_lighting_shader_property, block, controller, end);
	block->name_string = nif_get_string(nif, block->name);
	return block;
}

void *read_bs_shader_texture_set(nifr)
{
	//printf("read bs shader texture set\n");
	struct bs_shader_texture_set *block = malloc(sizeof(struct bs_shader_texture_set));
	ReadStruct(nif, bs_shader_texture_set, block, num_textures, textures);
	block->textures = malloc(sizeof(char *) * block->num_textures);
	memset(block->textures, '\0', block->num_textures);
	int l = block->num_textures;
	for (unsigned i = 0; i < l; i++)
	{
	block->textures[i] = nif_read_sized_string(nif);
	}
	return block;
}

void *read_ni_alpha_property(nifr)
{
	struct ni_alpha_property *block = malloc(sizeof(struct ni_alpha_property));
	ReadStruct(nif, ni_alpha_property, block, name, extra_data_list);
	ReadArray(nif, ni_alpha_property, block, ni_ref, extra_data_list, num_extra_data_list, 0);
	ReadStruct(nif, ni_alpha_property, block, controller, end);
	//read_ni_alpha_property2(nif, n, pos, Pos);
	return block;
}

void visit(struct rd *, int, int);
void visit_other(struct rd *, int, int);
void visit_block(struct rd *, void *);

api struct rd *nif_alloc_rd() {
	struct rd *rd = malloc(sizeof(struct rd));
	memset(rd, 0, sizeof(struct rd));
	rd->other = visit_other;
	rd->ni_node = rd->ni_tri_shape = rd->ni_tri_shape_data = rd->bs_lighting_shader_property = rd->bs_shader_texture_set = visit_block;
	return rd;
}

api void nif_free_rd(struct rd **p) {
	struct rd *rd = *p;
	free(rd->skips);
	free(rd);
	*p = NULL;
}

api void nif_rd(struct nif *nif, struct rd *rd) {
	// printf("nif rd\n");
	rd->skips = malloc(sizeof(int) * Hedr.num_blocks);
	memset(rd->skips, 0, sizeof(int) * Hedr.num_blocks);
	rd->nif = nif;
	for (int n = 0; n < Hedr.num_blocks; n++)
	{
	visit(rd, -1, n);
	}
}

void visit(struct rd *rd, int p, int c)
{
#define skip rd->skips[c] = 1;
#define needs_parent if (-1 == p) return;
	struct nif *nif = rd->nif;
	if (-1 == c)
	return;
	if (rd->skips[c])
	return;
	rd->parent = p; rd->current = c;
	// skip
	const char *block_type = Hedr.block_types[Hedr.block_type_index[c]];
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) )
	{
	skip;
	struct ni_node *block = Blocks[c];
	rd->ni_node(rd, block);
	for (int i = 0; i < block->num_children; i++)
	{
	int b = block->children[i];
	visit(rd, c, b);
	}
	}
	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) )
	{
	skip;
	struct ni_tri_shape *block = Blocks[c];
	rd->ni_tri_shape(rd, block);
	visit(rd, c, block->data);
	visit(rd, c, block->shader_property);
	visit(rd, c, block->alpha_property);
	}
	else if ( ni_is_type(NI_TRI_SHAPE_DATA) )
	{
	needs_parent;
	rd->ni_tri_shape_data(rd, Blocks[c]);
	}
	else if ( ni_is_type(BS_LIGHTING_SHADER_PROPERTY) )
	{
	needs_parent;
	struct bs_lighting_shader_property *block = Blocks[c];
	rd->bs_lighting_shader_property(rd, block);
	visit(rd, c, block->texture_set);
	}
	else if ( ni_is_type(BS_SHADER_TEXTURE_SET) )
	{
	needs_parent;
	rd->bs_shader_texture_set(rd, Blocks[c]);
	}
	else if ( ni_is_type(NI_ALPHA_PROPERTY) )
	{
	needs_parent;
	rd->ni_alpha_property(rd, Blocks[c]);
	}
}

void visit_other(struct rd *rd, int p, int c)
{

}

void visit_block(struct rd *rd, void *block)
{

}