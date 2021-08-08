// see header
#include "common.h"

#include "nifp.h"
#include "nitypes.h"

// ignore

int nifs = 0;
struct nifppair nifmap[5000];

api void nifp_save(void *key, Nifp *nif) {
	nif->num = nifs;
	nifmap[nifs].key = key;
	nifmap[nifs].value = nif;
	nifs++;
	assertm(nifs < 5000, "nifp overflow");
}

// ignore

api Nifp *nifp_saved(void *key) {
	for (int i = 0; i < nifs; i++)
	if (nifmap[i].key == key)
	return nifmap[i].value;
	return NULL;
}

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  Buf + Pos

#define FromBuf(x) *(x*)(Depos); Pos += sizeof(x); // only used twice remove

api char *nifp_get_string(Nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr->strings[i];
}

api char *nifp_get_block_type(Nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr->block_types[Hedr->block_type_index[i]];
}

api void *nifp_get_block(Nifp *nif, int i) {
	if (i == -1)
	return NULL;
	return Blocks[i];
}

api Nifp *calloc_nifp() {
	Nifp *nif = malloc(sizeof(Nifp));
	memset(nif, 0, sizeof(Nifp));
	nif->hdr = malloc(sizeof(struct nifp_hedr));
	return nif;
}

api void free_nifp(Nifp **p) {
	Nifp *nif = *p;
	free(nif);
	*p = NULL;
}

api void nifp_read(Nifp *nif) {
	assertm(Buf, "nifp buf is null ??");
	nifp_read_header(nif);
	nifp_read_blocks(nif);
}

static void read_short_string(Nifp *nif, char **string) {
	// has \0
	char len = FromBuf(char);
	*string = Depos;
	Pos += len;
}

static void read_sized_string(Nifp *nif, char **string) {
	// doesnt have \0
	unsigned int len = FromBuf(unsigned int);
	*string = malloc(sizeof(char) * len + 1);
	strncpy(*string, Depos, len);
	(*string)[len] = '\0';
	Pos += len;
}

// begin hedr

static void hedr_read_header_string(Nifp *nif) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int newline = (char *)memchr(Buf, 0x0A, 50) - Buf;
	assertm(newline, "nifp hedr");
	char *string = malloc(sizeof(char) * newline + 1);
	strncpy(string, Buf, newline);
	Hedr->header_string = string;
	Hedr->version = string + 30;
	Pos += newline + 1;
}

static void read_mem(Nifp *nif, void *dest, int size) {
	memcpy(dest, Depos, size);
	Pos += size;
}

static void read_array(Nifp *nif, void **dest, int size) {
	*dest = malloc(size);
	read_mem(nif, *dest, size);
}

static void read_sized_strings(Nifp *nif, char ***dest, int count) {
	*dest = malloc(count * sizeof(char *));
	for (int i = 0; i < count; i++)
	read_sized_string(nif, &(*dest)[i]);
}

api void nifp_read_header(Nifp *nif) {
	hedr_read_header_string(nif);
	read_mem(nif, &Hedr->unknown_1, 17);
	read_short_string(nif, &Hedr->author);
	read_short_string(nif, &Hedr->process_script);
	read_short_string(nif, &Hedr->export_script);
	read_mem(nif, &Hedr->num_block_types, 2);
	read_sized_strings(nif, &Hedr->block_types, Hedr->num_block_types);
	read_array(nif, &Hedr->block_type_index, sizeof(unsigned short) * Hedr->num_blocks);
	read_array(nif, &Hedr->block_sizes,      sizeof(unsigned int)   * Hedr->num_blocks);
	read_mem(nif, &Hedr->num_strings, 8);
	read_sized_strings(nif, &Hedr->strings, Hedr->num_strings);
	read_mem(nif, &Hedr->num_groups, 4);
	Hedr->end = Pos;
}

// read blockz

#define nifpr Nifp *nif, int n

static void read_block(Nifp *, int);

static void *read_ni_common_layout(nifpr);
static void *read_ni_node(nifpr);
static void *read_ni_skin_instance(nifpr);
static void *read_ni_skin_data(nifpr);
static void *read_ni_skin_partition(nifpr);
static void *read_ni_tri_shape(nifpr);
static void *read_ni_tri_shape_data(nifpr);
static void *read_bs_lighting_shader_property(nifpr);
static void *read_bs_effect_shader_property(nifpr);
static void *read_bs_shader_texture_set(nifpr);
static void *read_ni_alpha_property(nifpr);
static void *read_ni_controller_sequence(nifpr);
static void *read_ni_transform_interpolator(nifpr);
static void *read_ni_transform_data(nifpr);
#if ITS_THE_SPECIAL_EDITION
static void *read_bs_tri_shape(nifpr);
#endif

api void nifp_read_blocks(Nifp *nif)
{
	// printf("nifp path %s\n", nif->path);
	unsigned int pos = Pos;
	Blocks = malloc(sizeof(void *) * Hedr->num_blocks);
	for (unsigned int i = 0; i < Hedr->num_blocks; i++)
	{
	Blocks[i] = NULL;
	// printf("nifp block begin at %i %04x\n", Pos, Pos);
	read_block(nif, i);
	pos += Hedr->block_sizes[i];
	Pos = pos;
	}
}

void read_block(Nifp *nif, int n)
{
	const char *block_type = Hedr->block_types[Hedr->block_type_index[n]];
	void *block = NULL;
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) ) block = read_ni_node(nif, n);
	else if ( ni_is_any(NI_SKIN_INSTANCE, BS_DISMEMBER_SKIN_INSTANCE, NULL) ) block = read_ni_skin_instance(nif, n);
	else if ( ni_is_type(NI_SKIN_DATA) ) block = read_ni_skin_data(nif, n);
	else if ( ni_is_type(NI_SKIN_PARTITION) ) block = read_ni_skin_partition(nif, n);
#if ITS_THE_SPECIAL_EDITION
	else if ( ni_is_type(BS_TRI_SHAPE) ) block = read_bs_tri_shape(nif, n);
#endif
	else if ( ni_is_type(BS_DYNAMIC_TRI_SHAPE) ) 0;
	else if ( ni_is_any(NI_TRI_SHAPE, BS_LOD_TRI_SHAPE, NULL) ) block = read_ni_tri_shape(nif, n);
	else if ( ni_is_type(NI_ALPHA_PROPERTY) ) block = read_ni_alpha_property(nif, n);
	else if ( ni_is_type(NI_TRI_SHAPE_DATA) ) block = read_ni_tri_shape_data(nif, n);
	else if ( ni_is_type(BS_LIGHTING_SHADER_PROPERTY) ) block = read_bs_lighting_shader_property(nif, n);
	else if ( ni_is_type(BS_EFFECT_SHADER_PROPERTY) ) block = read_bs_effect_shader_property(nif, n);
	else if ( ni_is_type(BS_EFFECT_SHADER_PROPERTY_FLOAT_CONTROLLER) ) 0;
	else if ( ni_is_type(NI_FLOAT_INTERPOLATOR) ) 0;
	else if ( ni_is_type(NI_FLOAT_DATA) ) 0;
	else if ( ni_is_type(BS_SHADER_TEXTURE_SET) ) block = read_bs_shader_texture_set(nif, n);
	else if ( ni_is_type(NI_CONTROLLER_SEQUENCE) ) block = read_ni_controller_sequence(nif, n);
	else if ( ni_is_type(NI_TEXT_KEY_EXTRA_DATA) ) 0;
	else if ( ni_is_type(NI_STRING_EXTRA_DATA) ) 0;
	else if ( ni_is_type(NI_TRANSFORM_INTERPOLATOR) ) block = read_ni_transform_interpolator(nif, n); 
	else if ( ni_is_type(NI_TRANSFORM_DATA) ) block = read_ni_transform_data(nif, n);
	else if ( ni_is_type(BS_DECAL_PLACEMENT_VECTOR_EXTRA_DATA) ) 0;
	Blocks[n] = block;
}

static void sink_val(Nifp *nif, char **dest, int size) {
	*dest = Depos;
	Pos += size;
}

#define SinkOld(nif, block_pointer, layout, part, size) \
	sink_val(nif, (char *)block_pointer, offsetof(struct layout, part), size)

// (the magic here is sizeof *unary-expression)

#define Sink(nif, block, part) \
	sink_val(nif, &block->part, sizeof *block->part)

#define Sail(nif, block, part, group, num) \
	sink_val(nif, &block->part, sizeof *block->part * block->group->num)

#define CHEESE_CALLOC(type) type *block = calloc(1, sizeof(type))

#define Arr(count, type) count * sizeof(type)

void *read_ni_common_layout(nifpr)
{
	CHEESE_CALLOC(struct ni_common_layout_pointer);
	Sink(nif, block, F);
	Sail(nif, block, extra_data_list, F, num_extra_data_list);
	Sink(nif, block, A);
	return block;
}

void *read_ni_node(nifpr)
{
	CHEESE_CALLOC(struct ni_node_pointer);
	block->common = read_ni_common_layout(nif, n);
	Sink(nif, block, A);
	Sail(nif, block, children, A, num_children);
	Sink(nif, block, B);
	Sail(nif, block, effects, B, num_effects);
	return block;
}

void *read_bs_tri_shape(Nifp *nif, int n)
{
	printf("read bs tri shape for %s!\n", nif->path);
	CHEESE_CALLOC(struct bs_tri_shape_pointer);
	block->common = read_ni_common_layout(nif, n);
	Sink(nif, block, bounding_sphere);
	Sink(nif, block, refs);
	printf("skin shader alpha %i %i %i\n", block->refs->skin, block->refs->shader_property, block->refs->alpha_property);
	Sink(nif, block, infos);
	printf("num triangles vertices %hu %hu\n", block->infos->num_triangles, block->infos->num_vertices);
	printf("data size %u\n", block->infos->data_size);
	Sail(nif, block, vertex_data, infos, num_vertices);
	Sail(nif, block, triangles, infos, num_triangles);
	Sink(nif, block, particle_data_size);

	printf("&block->vertex_data[0] = %i\n", &block->vertex_data[0]);
	printf("&block->vertex_data[1] = %i\n", &block->vertex_data[1]);
	sizeof(struct bs_vertex_data_sse);
	struct bs_vertex_data_sse *zero = &block->vertex_data[0];
	printf("bs_vertex_data_sse[0] vertex x is %f\n", zero->vertex.x);
	printf("bs_vertex_data_sse[0] bitangent x is %f\n", zero->bitangent_x);
	printf("bs_vertex_data_sse[0] normal is %u\n", zero->normal.x);
	printf("bs_vertex_data_sse[0] bitangent y is %u\n", zero->bitangent_y);
	printf("bs_vertex_data_sse[0] bitangent z is %u\n", zero->bitangent_z);
	printf("bs_vertex_data_sse[0] vertex_colors is %u %u %u %u\n", zero->vertex_colors.r, zero->vertex_colors.g, zero->vertex_colors.b, zero->vertex_colors.a);

	printf("particle data size %u\n", *block->particle_data_size);
}

void *read_ni_tri_shape(nifpr)
{
	printf("read ni tri shape pointer\n");
	// assertm(0, "sse shouldnt use ni tri shape");
	CHEESE_CALLOC(struct ni_tri_shape_pointer);
	block->common = read_ni_common_layout(nif, n);
	Sink(nif, block, A);
	Pos += 9;
	Sink(nif, block, B);
	return block;
}

void *read_ni_tri_shape_data(nifpr)
{
	CHEESE_CALLOC(struct ni_tri_shape_data_pointer);
	
	Sink(nif, block, A);

	if (block->A->has_vertices)
		Sail(nif, block, vertices, A, num_vertices);

	Sink(nif, block, C);

	if (block->C->has_normals)
		Sail(nif, block, normals, A, num_vertices);

	if (block->C->bs_vector_flags & 0x00001000)
	{
		Sail(nif, block, tangents, A, num_vertices);
		Sail(nif, block, bitangents, A, num_vertices);
	}

	Sink(nif, block, G);

	if (block->G->has_vertex_colors)
		Sail(nif, block, vertex_colors, A, num_vertices);

	if (block->C->bs_vector_flags & 0x00000001)
		Sail(nif, block, uv_sets, A, num_vertices);

	Sink(nif, block, J);

	if (block->J->has_triangles)
		Sail(nif, block, triangles, J, num_triangles);

	Sink(nif, block, L);

	Sail(nif, block, match_groups, L, num_match_groups);

	return block;
}

void *read_ni_skin_instance(nifpr)
{
	struct ni_skin_instance_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_skin_instance_pointer));
	memset(block_pointer, 0, sizeof(struct ni_skin_instance_pointer));
	SinkOld(nif, block_pointer, ni_skin_instance_pointer, A, 16);
	SinkOld(nif, block_pointer, ni_skin_instance_pointer, bones,
		Arr(block_pointer->A->num_bones, ni_ref));
	char *block_type = nifp_get_block_type(nif, n);
	if (ni_is_type(BS_DISMEMBER_SKIN_INSTANCE))
	{
	SinkOld(nif, block_pointer, ni_skin_instance_pointer, B, 4);
	SinkOld(nif, block_pointer, ni_skin_instance_pointer, partitions,
		Arr(block_pointer->B->num_partitions, struct body_part_list));
	}
	return block_pointer;
}

void *read_ni_skin_data(nifpr)
{
#define Type ni_skin_data_pointer
	//printf("read ni skin data\n");
	struct Type *block_pointer;
	block_pointer = malloc(sizeof(struct Type));
	memset(block_pointer, 0, sizeof(struct Type));
	SinkOld(nif, block_pointer, Type, skin_transform, 36+12+4);
	SinkOld(nif, block_pointer, Type, B, 5);
	block_pointer->bone_list = malloc(sizeof(struct bone_data *) * block_pointer->B->num_bones);
	for (unsigned int i = 0; i < block_pointer->B->num_bones; i++)
	{
	block_pointer->bone_list[i] = malloc(sizeof(struct bone_data));
	struct bone_data *bone_data = block_pointer->bone_list[i];
	SinkOld(nif, bone_data, bone_data, skin_transform, 36+12+4);
	SinkOld(nif, bone_data, bone_data, B, 12+4+2);
	SinkOld(nif, bone_data, bone_data, vertex_weights,
		Arr(bone_data->B->num_vertices, struct bone_vert_data));
	}
	return block_pointer;
}

void *read_ni_skin_partition(nifpr)
{
	//printf("read ni skin partition\n");
	struct ni_skin_partition_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_skin_partition_pointer));
	memset(block_pointer, 0, sizeof(struct ni_skin_partition_pointer));
	SinkOld(nif, block_pointer, ni_skin_partition_pointer, num_skin_partition_blocks, 4);
	block_pointer->skin_partition_blocks = malloc(sizeof(struct skin_partition *) * *block_pointer->num_skin_partition_blocks);
	for (unsigned int i = 0; i < *block_pointer->num_skin_partition_blocks; i++)
	{
	block_pointer->skin_partition_blocks[i] = malloc(sizeof(struct skin_partition));
	struct skin_partition *skin_partition = block_pointer->skin_partition_blocks[i];
	SinkOld(nif, skin_partition, skin_partition, A, 10);
	SinkOld(nif, skin_partition, skin_partition, bones,
		Arr(skin_partition->A->num_bones, unsigned short));
	SinkOld(nif, skin_partition, skin_partition, has_vertex_map, 1);
	if (*skin_partition->has_vertex_map)
	SinkOld(nif, skin_partition, skin_partition, vertex_map,
		Arr(skin_partition->A->num_vertices, unsigned short));
	SinkOld(nif, skin_partition, skin_partition, has_vertex_weights, 1);
	if (*skin_partition->has_vertex_weights)
	SinkOld(nif, skin_partition, skin_partition, vertex_weights,
		Arr(skin_partition->A->num_vertices, struct vec_4p));
	SinkOld(nif, skin_partition, skin_partition, strip_lengths,
		Arr(skin_partition->A->num_strips, unsigned short));
	SinkOld(nif, skin_partition, skin_partition, has_faces, 1);
	if (*skin_partition->has_faces)
	SinkOld(nif, skin_partition, skin_partition, triangles,
		Arr(skin_partition->A->num_triangles, struct ushort_3p));
	SinkOld(nif, skin_partition, skin_partition, has_bone_indices, 1);
	if (*skin_partition->has_bone_indices)
	SinkOld(nif, skin_partition, skin_partition, bone_indices,
		Arr(skin_partition->A->num_vertices, struct byte_4p));
	SinkOld(nif, skin_partition, skin_partition, unknown_short, 2);
	}
	return block_pointer;
};

void *read_bs_lighting_shader_property(nifpr)
{
	CHEESE_CALLOC(struct bs_lighting_shader_property_pointer);
	Sink(nif, block, A);
	Sail(nif, block, extra_data_list, A, num_extra_data_list);
	Sink(nif, block, B);
	return block;
}

void *read_bs_effect_shader_property(nifpr)
{
	CHEESE_CALLOC(struct bs_effect_shader_property_pointer);
	Sink(nif, block, A);
	Sail(nif, block, extra_data_list, A, num_extra_data_list);
	Sink(nif, block, B);
	read_sized_string(nif, &block->source_texture);
	Sink(nif, block, C);
	read_sized_string(nif, &block->greyscale_texture);
	return block;
}

void *read_bs_shader_texture_set(nifpr)
{
	CHEESE_CALLOC(struct bs_shader_texture_set_pointer);
	Sink(nif, block, A);
	read_sized_strings(nif, &block->textures, block->A->num_textures);
	return block;
}

void *read_ni_alpha_property(nifpr)
{
	CHEESE_CALLOC(struct ni_alpha_property_pointer);
	Sink(nif, block, A);
	Sail(nif, block, extra_data_list, A, num_extra_data_list);
	Sink(nif, block, C);
	return block;
}

void *read_ni_controller_sequence(nifpr)
{
	//printf("read_ni_controller_sequence\n");
	struct ni_controller_sequence_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_controller_sequence_pointer));
	memset(block_pointer, 0, sizeof(struct ni_controller_sequence_pointer));
	SinkOld(nif, block_pointer, ni_controller_sequence_pointer, A, 12);
	SinkOld(nif, block_pointer, ni_controller_sequence_pointer, controlled_blocks,
		Arr(block_pointer->A->num_controlled_blocks, struct controlled_block_pointer));
	SinkOld(nif, block_pointer, ni_controller_sequence_pointer, C, sizeof(((struct ni_controller_sequence_pointer *)0)->C));
	//printf("ni ct se num_controlled_blocks %u\n", block_pointer->A->num_controlled_blocks);
	sizeof(struct ni_controller_sequence_pointer);
	sizeof(((struct ni_controller_sequence_pointer *)0)->A);
	return block_pointer;
}

void *read_ni_transform_interpolator(nifpr)
{
	//printf("read_ni_transform_interpolator\n");
	struct ni_transform_interpolator_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_transform_interpolator_pointer));
	memset(block_pointer, 0, sizeof(struct ni_transform_interpolator_pointer));
	SinkOld(nif, block_pointer, ni_transform_interpolator_pointer, transform, 32);
	SinkOld(nif, block_pointer, ni_transform_interpolator_pointer, B, 4);
	return block_pointer;
}

void *read_ni_transform_data(nifpr)
{
	//printf("read_ni_transform_data\n");
	struct ni_transform_data_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_transform_data_pointer));
	memset(block_pointer, 0, sizeof(struct ni_transform_data_pointer));
	SinkOld(nif, block_pointer, ni_transform_data_pointer, A, 4);
	if (block_pointer->A->num_rotation_keys > 0)
	{
	SinkOld(nif, block_pointer, ni_transform_data_pointer, B, 4);
	if (block_pointer->B->rotation_type == 2) // quadratic key
	{
	SinkOld(nif, block_pointer, ni_transform_data_pointer, quaternion_keys,
		Arr(block_pointer->A->num_rotation_keys, struct quaternion_key_pointer));
	}
	else
	{
	printf("0x1");
	//printf("cant read this rotation type or quatkey type");
	return NULL;
	}
	}
	SinkOld(nif, block_pointer, ni_transform_data_pointer, translations, 8);
	SinkOld(nif, block_pointer, ni_transform_data_pointer, translation_keys,
		Arr(block_pointer->translations->num_keys, struct translation_key_pointer));
	SinkOld(nif, block_pointer, ni_transform_data_pointer, scales, 4);
	SinkOld(nif, block_pointer, ni_transform_data_pointer, scale_keys,
		Arr(block_pointer->scales->num_keys, 4));
	return block_pointer;
}
