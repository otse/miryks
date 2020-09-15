// nif but with pointers

#include "putc.h"

#include "nifp.h"

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  Buf + Pos

#define FromBuf(x) *(x*)(Depos); Pos += sizeof(x); // todo remove

api char *nifp_get_string(struct nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr.strings[i];
}

api char *nifp_get_block_type(struct nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr.block_types[Hedr.block_type_index[i]];
}

api void *nifp_get_block(struct nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Blocks[i];
}

api struct nifp *malloc_nifp() {
	struct nifp *nif = malloc(sizeof(struct nifp));
	Pos = 0;
	Buf = 0;
	return nif;
}

api void nifp_read(struct nifp *nif) {
	cassert(Buf, "nifp_read Buf not set");
	nifp_read_header(nif);
	nifp_read_blocks(nif);
}

static void read_short_string(struct nifp *nif, char **string) {
	char len = FromBuf(char);
	*string = malloc(sizeof(char) * len);
	strncpy(*string, Depos, len);
	Pos += len;
}

static void read_sized_string(struct nifp *nif, char **string) {
	unsigned int len = FromBuf(unsigned int);
	*string = malloc(sizeof(char) * len + 1);
	strncpy(*string, Depos, len);
	(*string)[len] = '\0'; // ?
	Pos += len;
}

// begin hedr

static void hedr_read_header_string(struct nifp *nif) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(Buf, '\n') - Buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, Buf, n);
	string[n - 1] = '\0';
	Hedr.header_string = string;
	Hedr.version = string + 30;
	Pos += n;
}

static void read_value(struct nifp *nif, void *dest, int size) {
	memcpy(dest, Depos, size);
	Pos += size;
}

static void read_array(struct nifp *nif, void **dest, int size) {
	*dest = malloc(size);
	read_value(nif, *dest, size);
}

static void read_sized_strings(struct nifp *nif, char ***dest, int count) {
	*dest = malloc(count * sizeof(char *));
	for (int i = 0; i < count; i++)
	read_sized_string(nif, &(*dest)[i]);
}

api void nifp_read_header(struct nifp *nif) {
	hedr_read_header_string(nif);
	read_value(nif, &Hedr.unknown_1, 17);
	read_short_string(nif, &Hedr.author);
	read_short_string(nif, &Hedr.process_script);
	read_short_string(nif, &Hedr.export_script);
	read_value(nif, &Hedr.num_block_types, 2);
	read_sized_strings(nif, &Hedr.block_types, Hedr.num_block_types);
	read_array(nif, &Hedr.block_type_index, sizeof(unsigned short) * Hedr.num_blocks);
	read_array(nif, &Hedr.block_sizes,      sizeof(unsigned int)   * Hedr.num_blocks);
	read_value(nif, &Hedr.num_strings, 8);
	read_sized_strings(nif, &Hedr.strings, Hedr.num_strings);
	read_value(nif, &Hedr.num_groups, 4);
	Hedr.end = Pos;
}

// read blockz

#define nifpr struct nifp *nif, int n

static void read_block(struct nifp *, int);

static void *read_ni_common_layout(nifpr);
static void *read_ni_node(nifpr);
static void *read_ni_skin_instance(nifpr);
static void *read_ni_skin_data(nifpr);
static void *read_ni_tri_shape(nifpr);
static void *read_ni_tri_shape_data(nifpr);
static void *read_bs_lighting_shader_property(nifpr);
static void *read_bs_shader_texture_set(nifpr);
static void *read_ni_alpha_property(nifpr);

api void nifp_read_blocks(struct nifp *nif)
{
	printf("nifp path %s\n", nif->path);
	unsigned int pos = Pos;
	Blocks = malloc(sizeof(void *) * Hedr.num_blocks);
	for (unsigned int i = 0; i < Hedr.num_blocks; i++)
	{
	Blocks[i] = NULL;
	// printf("nifp block begin at %i %04x\n", Pos, Pos);
	read_block(nif, i);
	pos += Hedr.block_sizes[i];
	Pos = pos;
	}
}

void read_block(struct nifp *nif, int n)
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

static void sink_val(struct nifp *nif, char *block_pointer, int src, int size) {
	char **dest = block_pointer + src;
	*dest = Depos;
	Pos += size;
}

// sizeof(((layout *)0)->part)
#define SinkVal(nif, block_pointer, layout, part, size) sink_val(nif, block_pointer, offsetof(layout, part), size)
//#define SinkArr(nif, block_pointer, layout, part, size) sink_arr(nif, block_pointer, offsetof(layout, part), size)

#define ArrSize(count, type) count * sizeof(type)

void *read_ni_common_layout(nifpr)
{
	printf("read nifp block %i\n", n);
	struct ni_common_layout_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_common_layout_pointer));
	memset(block_pointer, NULL, sizeof(struct ni_common_layout_pointer));
	SinkVal(nif, block_pointer, struct ni_common_layout_pointer, A, 8);
	printf("ni cmmon stuff name %i %u\n", block_pointer->A->name, block_pointer->A->num_extra_data_list);
	SinkVal(nif, block_pointer, struct ni_common_layout_pointer, B, ArrSize(block_pointer->A->num_extra_data_list, ni_ref));
	SinkVal(nif, block_pointer, struct ni_common_layout_pointer, C, 4+4+12+36+4+4);
	return block_pointer;
}

void *read_ni_node(nifpr)
{
	//printf("read ni node pointer\n");
	struct ni_node_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_node_pointer));
	memset(block_pointer, NULL, sizeof(struct ni_node_pointer));

	block_pointer->common = read_ni_common_layout(nif, n);
	SinkVal(nif, block_pointer, struct ni_node_pointer, A, 4);
	SinkVal(nif, block_pointer, struct ni_node_pointer, children, ArrSize(block_pointer->A->num_children, ni_ref));
	SinkVal(nif, block_pointer, struct ni_node_pointer, C, 4);
	SinkVal(nif, block_pointer, struct ni_node_pointer, effects, ArrSize(block_pointer->C->num_effects, ni_ref));
	return block_pointer;
}

void *read_ni_tri_shape(nifpr)
{
	printf("read ni tri shape pointer\n");
	struct ni_tri_shape_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_tri_shape_pointer));
	memset(block_pointer, NULL, sizeof(struct ni_tri_shape_pointer));
	block_pointer->common = read_ni_common_layout(nif, n);
	SinkVal(nif, block_pointer, struct ni_tri_shape_pointer, A, 8);
	Pos += 9;
	SinkVal(nif, block_pointer, struct ni_tri_shape_pointer, B, 8);
	return block_pointer;
}

void *read_ni_tri_shape_data(nifpr)
{
	printf("read ni tri shape data\n");
	struct ni_tri_shape_data_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_tri_shape_data_pointer));
	memset(block_pointer, NULL, sizeof(struct ni_tri_shape_data_pointer));
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, A, 4+2+1+1+1);
	const int size = ArrSize(block_pointer->A->num_vertices, struct vec_3p);
	if (block_pointer->A->has_vertices)
	{
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, vertices, size);
	}
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, C, 7);
	if (block_pointer->C->has_normals)
	{
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, normals, size);
	}
	if (block_pointer->C->bs_vector_flags & 0x00001000)
	{
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, tangents, size);
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, bitangents, size);
	}
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, G, 12+4+1);
	if (block_pointer->G->has_vertex_colors)
	{
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, vertex_colors, ArrSize(block_pointer->A->num_vertices, struct vec_4p));
	}
	if (block_pointer->C->bs_vector_flags & 0x00000001)
	{
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, uv_sets, ArrSize(block_pointer->A->num_vertices, struct vec_2p));
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, J, 13);
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, triangles, ArrSize(block_pointer->J->num_triangles, struct ushort_3p));
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, L, 2);
	SinkVal(nif, block_pointer, struct ni_tri_shape_data_pointer, match_groups, ArrSize(block_pointer->L->num_match_groups, ni_ref));
	}
	Blocks[n] = block_pointer;
	char str[1000];
	nifp_print_block(nif, n, str);
	printf(str);
	return block_pointer;
}

void *read_ni_skin_instance(nifpr)
{
	//read_block_save(nif, n, block);
	return NULL;
}

void *read_ni_skin_data(nifpr)
{
	//read_block_save(nif, n, block);
	return NULL;
}

void *read_bs_lighting_shader_property(nifpr)
{
	//printf("read bs lighting shader property\n");
	struct bs_lighting_shader_property_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct bs_lighting_shader_property_pointer));
	memset(block_pointer, NULL, sizeof(struct bs_lighting_shader_property_pointer));
	/*ReadStruct(nif, bs_lighting_shader_property, block, skyrim_shader_type, extra_data_list);
	ReadArray(nif, bs_lighting_shader_property, block, ni_ref, extra_data_list, num_extra_data_list, 0);
	ReadStruct(nif, bs_lighting_shader_property, block, controller, end);*/
	//block->name_string = nif_get_string(nif, block->name);
	return block_pointer;
}

void *read_bs_shader_texture_set(nifpr)
{
	//printf("read bs shader texture set\n");
	struct bs_shader_texture_set_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct bs_shader_texture_set_pointer));
	memset(block_pointer, NULL, sizeof(struct bs_shader_texture_set_pointer));
	/*ReadStruct(nif, bs_shader_texture_set, block, num_textures, textures);
	block->textures = malloc(sizeof(char *) * block->num_textures);
	memset(block->textures, '\0', block->num_textures);
	int l = block->num_textures;
	for (unsigned i = 0; i < l; i++)
	{
	block->textures[i] = nif_read_sized_string(nif);
	}*/
	return block_pointer;
}

void *read_ni_alpha_property(nifpr)
{
	printf("nifp exp\n");
	struct ni_alpha_property_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_alpha_property_pointer));
	memset(block_pointer, NULL, sizeof(struct ni_alpha_property_pointer));
	//int of = offsetof(struct ni_alpha_property_pointer, A);
	//printf("offsetof(struct ni_alpha_property_pointer, A) %i\n", of);
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, A, 8);
	printf("fields from A %i %u\n", block_pointer->A->name, block_pointer->A->num_extra_data_list);
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, extra_data_list, ArrSize(block_pointer->A->num_extra_data_list, ni_ref));
	SinkVal(nif, block_pointer, struct ni_alpha_property_pointer, C, 7);
	printf("fields from B %i %hu %u\n", block_pointer->C->controller, block_pointer->C->flags, block_pointer->C->treshold);
	sizeof(struct ni_alpha_property_pointer);
	sizeof(((struct ni_alpha_property_pointer *)0)->C);
	return block_pointer;
}