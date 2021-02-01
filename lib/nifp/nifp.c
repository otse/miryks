// nif but with pointers

// wrote for gloom/dark
// for oldrim

#include "putc.h"

#include "nifp.h"
#include "nitypes.h"

int nifps = 0;
struct nifppair nifpmap[5000];

api void nifp_save(void *key, Nifp *nif) {
	nif->key = nifps;
	nifpmap[nifps].key = key;
	nifpmap[nifps].value = nif;
	nifps++;
	cassert(nifps<5000, "nifps > 5000");
}

api Nifp *nifp_saved(void *key) {
	for (int i = 0; i < nifps; i++)
	if (nifpmap[i].key == key)
	return nifpmap[i].value;
	return NULL;
}

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  Buf + Pos

#define FromBuf(x) *(x*)(Depos); Pos += sizeof(x); // todo remove

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

api Nifp *malloc_nifp() {
	Nifp *nif = malloc(sizeof(Nifp));
	Pos = 0;
	Buf = 0;
	nif->hdr = malloc(sizeof(struct nifp_hedr));
	return nif;
}

api void free_nifp(Nifp **p) {
	Nifp *nif = *p;
	free(nif);
	*p = NULL;
}

api void nifp_read(Nifp *nif) {
	cassert(Buf, "nifp buf is null");
	nifp_read_header(nif);
	nifp_read_blocks(nif);
}

static void read_short_string(Nifp *nif, char **string) {
	char len = FromBuf(char);
	*string = malloc(sizeof(char) * len);
	strncpy(*string, Depos, len);
	Pos += len;
}

static void read_sized_string(Nifp *nif, char **string) {
	unsigned int len = FromBuf(unsigned int);
	*string = malloc(sizeof(char) * len + 1);
	strncpy(*string, Depos, len);
	(*string)[len] = '\0'; // ?
	Pos += len;
}

// begin hedr

static void hedr_read_header_string(Nifp *nif) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(Buf, '\n') - Buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, Buf, n);
	string[n - 1] = '\0';
	Hedr->header_string = string;
	Hedr->version = string + 30;
	Pos += n;
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
	else if ( ni_is_type(BS_TRI_SHAPE) ) 0;
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

static void sink_val(Nifp *nif, char *block_pointer, int src, int size) {
	char **dest = block_pointer + src;
	*dest = Depos;
	Pos += size;
}

// sizeof(((layout *)0)->part)
#define Sink(nif, block_pointer, layout, part, size) sink_val(nif, (char *)block_pointer, offsetof(struct layout, part), size)
//#define SinkArr(nif, block_pointer, layout, part, size) sink_arr(nif, block_pointer, offsetof(layout, part), size)

#define Arr(count, type) count * sizeof(type)

void *read_ni_common_layout(nifpr)
{
	// printf("read nifp commons %i\n", n);
	struct ni_common_layout_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_common_layout_pointer));
	memset(block_pointer, 0, sizeof(struct ni_common_layout_pointer));
	Sink(nif, block_pointer, ni_common_layout_pointer, A, 8);
	Sink(nif, block_pointer, ni_common_layout_pointer, extra_data_list, Arr(block_pointer->A->num_extra_data_list, ni_ref));
	Sink(nif, block_pointer, ni_common_layout_pointer, C, 4+4+12+36+4+4);
	return block_pointer;
}

void *read_ni_node(nifpr)
{
	//printf("read ni node pointer\n");
	struct ni_node_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_node_pointer));
	memset(block_pointer, 0, sizeof(struct ni_node_pointer));
	block_pointer->common = read_ni_common_layout(nif, n);
	Sink(nif, block_pointer, ni_node_pointer, A, 4);
	Sink(nif, block_pointer, ni_node_pointer, children, Arr(block_pointer->A->num_children, ni_ref));
	Sink(nif, block_pointer, ni_node_pointer, C, 4);
	Sink(nif, block_pointer, ni_node_pointer, effects, Arr(block_pointer->C->num_effects, ni_ref));
	return block_pointer;
}

void *read_ni_tri_shape(nifpr)
{
	// printf("read ni tri shape pointer\n");
	struct ni_tri_shape_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_tri_shape_pointer));
	memset(block_pointer, 0, sizeof(struct ni_tri_shape_pointer));
	block_pointer->common = read_ni_common_layout(nif, n);
	Sink(nif, block_pointer, ni_tri_shape_pointer, A, 8);
	Pos += 9;
	Sink(nif, block_pointer, ni_tri_shape_pointer, B, 8);
	return block_pointer;
}

void *read_ni_tri_shape_data(nifpr)
{
	// printf("read ni tri shape data\n");
	struct ni_tri_shape_data_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_tri_shape_data_pointer));
	memset(block_pointer, 0, sizeof(struct ni_tri_shape_data_pointer));
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, A, 4+2+1+1+1);
	const int size = Arr(block_pointer->A->num_vertices, struct vec_3p);
	if (block_pointer->A->has_vertices)
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, vertices, size);
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, C, 7);
	if (block_pointer->C->has_normals)
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, normals, size);
	if (block_pointer->C->bs_vector_flags & 0x00001000)
	{
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, tangents, size);
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, bitangents, size);
	}
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, G, 12+4+1);
	if (block_pointer->G->has_vertex_colors)
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, vertex_colors, Arr(block_pointer->A->num_vertices, struct vec_4p));
	if (block_pointer->C->bs_vector_flags & 0x00000001)
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, uv_sets, Arr(block_pointer->A->num_vertices, struct vec_2p));
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, J, 13);
	if (block_pointer->J->has_triangles)
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, triangles, Arr(block_pointer->J->num_triangles, struct ushort_3p));
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, L, 2);
	Sink(nif, block_pointer, ni_tri_shape_data_pointer, match_groups, Arr(block_pointer->L->num_match_groups, ni_ref));
	return block_pointer;
}

void *read_ni_skin_instance(nifpr)
{
	struct ni_skin_instance_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_skin_instance_pointer));
	memset(block_pointer, 0, sizeof(struct ni_skin_instance_pointer));
	Sink(nif, block_pointer, ni_skin_instance_pointer, A, 16);
	Sink(nif, block_pointer, ni_skin_instance_pointer, bones, Arr(block_pointer->A->num_bones, ni_ref));
	char *block_type = nifp_get_block_type(nif, n);
	if (ni_is_type(BS_DISMEMBER_SKIN_INSTANCE))
	{
	Sink(nif, block_pointer, ni_skin_instance_pointer, B, 4);
	Sink(nif, block_pointer, ni_skin_instance_pointer, partitions, Arr(block_pointer->B->num_partitions, struct body_part_list));
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
	Sink(nif, block_pointer, Type, skin_transform, 36+12+4);
	Sink(nif, block_pointer, Type, B, 5);
	block_pointer->bone_list = malloc(sizeof(struct bone_data *) * block_pointer->B->num_bones);
	for (int i = 0; i < block_pointer->B->num_bones; i++)
	{
	block_pointer->bone_list[i] = malloc(sizeof(struct bone_data));
	struct bone_data *bone_data = block_pointer->bone_list[i];
	Sink(nif, bone_data, bone_data, skin_transform, 36+12+4);
	Sink(nif, bone_data, bone_data, B, 12+4+2);
	Sink(nif, bone_data, bone_data, vertex_weights, Arr(bone_data->B->num_vertices, struct bone_vert_data));
	}
	return block_pointer;
}

void *read_ni_skin_partition(nifpr)
{
	//printf("read ni skin partition\n");
	struct ni_skin_partition_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_skin_partition_pointer));
	memset(block_pointer, 0, sizeof(struct ni_skin_partition_pointer));
	Sink(nif, block_pointer, ni_skin_partition_pointer, num_skin_partition_blocks, 4);
	block_pointer->skin_partition_blocks = malloc(sizeof(struct skin_partition *) * *block_pointer->num_skin_partition_blocks);
	for (unsigned int i = 0; i < *block_pointer->num_skin_partition_blocks; i++)
	{
	block_pointer->skin_partition_blocks[i] = malloc(sizeof(struct skin_partition));
	struct skin_partition *skin_partition = block_pointer->skin_partition_blocks[i];
	Sink(nif, skin_partition, skin_partition, A, 10);
	Sink(nif, skin_partition, skin_partition, bones, Arr(skin_partition->A->num_bones, unsigned short));
	Sink(nif, skin_partition, skin_partition, has_vertex_map, 1);
	if (*skin_partition->has_vertex_map)
	Sink(nif, skin_partition, skin_partition, vertex_map, Arr(skin_partition->A->num_vertices, unsigned short));
	Sink(nif, skin_partition, skin_partition, has_vertex_weights, 1);
	if (*skin_partition->has_vertex_weights)
	Sink(nif, skin_partition, skin_partition, vertex_weights, Arr(skin_partition->A->num_vertices, struct vec_4p));
	Sink(nif, skin_partition, skin_partition, strip_lengths, Arr(skin_partition->A->num_strips, unsigned short));
	Sink(nif, skin_partition, skin_partition, has_faces, 1);
	if (*skin_partition->has_faces)
	Sink(nif, skin_partition, skin_partition, triangles, Arr(skin_partition->A->num_triangles, struct ushort_3p));
	Sink(nif, skin_partition, skin_partition, has_bone_indices, 1);
	if (*skin_partition->has_bone_indices)
	Sink(nif, skin_partition, skin_partition, bone_indices, Arr(skin_partition->A->num_vertices, struct byte_4p));
	Sink(nif, skin_partition, skin_partition, unknown_short, 2);
	}
	return block_pointer;
};

void *read_bs_lighting_shader_property(nifpr)
{
	//printf("read bs lighting shader property\n");
	struct bs_lighting_shader_property_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct bs_lighting_shader_property_pointer));
	memset(block_pointer, 0, sizeof(struct bs_lighting_shader_property_pointer));
	Sink(nif, block_pointer, bs_lighting_shader_property_pointer, A, 12);
	Sink(nif, block_pointer, bs_lighting_shader_property_pointer, extra_data_list, Arr(block_pointer->A->num_extra_data_list, ni_ref));
	Sink(nif, block_pointer, bs_lighting_shader_property_pointer, B, 88);
	return block_pointer;
}

void *read_bs_effect_shader_property(nifpr)
{
	printf(" read_bs_effect_shader_property\n ");
	struct bs_effect_shader_property_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct bs_effect_shader_property_pointer));
	memset(block_pointer, 0, sizeof(struct bs_effect_shader_property_pointer));
	Sink(nif, block_pointer, bs_effect_shader_property_pointer, A, 8);
	Sink(nif, block_pointer, bs_effect_shader_property_pointer, extra_data_list, Arr(block_pointer->A->num_extra_data_list, ni_ref));
	Sink(nif, block_pointer, bs_effect_shader_property_pointer, B, 12+8+8);
	read_sized_string(nif, &block_pointer->source_texture);
	Sink(nif, block_pointer, bs_effect_shader_property_pointer, C, 4+16+16+8);
	read_sized_string(nif, &block_pointer->greyscale_texture);
	return block_pointer;
}

void *read_bs_shader_texture_set(nifpr)
{
	//printf("read bs shader texture set\n");
	struct bs_shader_texture_set_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct bs_shader_texture_set_pointer));
	memset(block_pointer, 0, sizeof(struct bs_shader_texture_set_pointer));
	Sink(nif, block_pointer, bs_shader_texture_set_pointer, A, 4);
	read_sized_strings(nif, &block_pointer->textures, block_pointer->A->num_textures);
	return block_pointer;
}

void *read_ni_alpha_property(nifpr)
{
	// printf("nifp exp\n");
	struct ni_alpha_property_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_alpha_property_pointer));
	memset(block_pointer, 0, sizeof(struct ni_alpha_property_pointer));
	Sink(nif, block_pointer, ni_alpha_property_pointer, A, 8);
	Sink(nif, block_pointer, ni_alpha_property_pointer, extra_data_list, Arr(block_pointer->A->num_extra_data_list, ni_ref));
	Sink(nif, block_pointer, ni_alpha_property_pointer, C, 7);
	sizeof(struct ni_alpha_property_pointer);
	sizeof(((struct ni_alpha_property_pointer *)0)->C);
	return block_pointer;
}

void *read_ni_controller_sequence(nifpr)
{
	//printf("read_ni_controller_sequence\n");
	struct ni_controller_sequence_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_controller_sequence_pointer));
	memset(block_pointer, 0, sizeof(struct ni_controller_sequence_pointer));
	Sink(nif, block_pointer, ni_controller_sequence_pointer, A, 12);
	Sink(nif, block_pointer, ni_controller_sequence_pointer, controlled_blocks, Arr(block_pointer->A->num_controlled_blocks, struct controlled_block_pointer));
	Sink(nif, block_pointer, ni_controller_sequence_pointer, C, sizeof(((struct ni_controller_sequence_pointer *)0)->C));
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
	Sink(nif, block_pointer, ni_transform_interpolator_pointer, transform, 32);
	Sink(nif, block_pointer, ni_transform_interpolator_pointer, B, 4);
	return block_pointer;
}

void *read_ni_transform_data(nifpr)
{
	//printf("read_ni_transform_data\n");
	struct ni_transform_data_pointer *block_pointer;
	block_pointer = malloc(sizeof(struct ni_transform_data_pointer));
	memset(block_pointer, 0, sizeof(struct ni_transform_data_pointer));
	Sink(nif, block_pointer, ni_transform_data_pointer, A, 4);
	if (block_pointer->A->num_rotation_keys > 0)
	{
	Sink(nif, block_pointer, ni_transform_data_pointer, B, 4);
	if (block_pointer->B->rotation_type == 2) // quadratic key
	{
	Sink(nif, block_pointer, ni_transform_data_pointer, quaternion_keys, Arr(block_pointer->A->num_rotation_keys, struct quaternion_key_pointer));
	}
	else
	{
	printf("cant read this rotation type or quatkey type\n");
	return NULL;
	}
	}
	Sink(nif, block_pointer, ni_transform_data_pointer, translations, 8);
	Sink(nif, block_pointer, ni_transform_data_pointer, translation_keys, Arr(block_pointer->translations->num_keys, struct translation_key_pointer));
	Sink(nif, block_pointer, ni_transform_data_pointer, scales, 4);
	Sink(nif, block_pointer, ni_transform_data_pointer, scale_keys, Arr(block_pointer->scales->num_keys, 4));
	return block_pointer;
}