/// nif.c

#include "common.h"

#include "nif.h"
#include "niftypes.h"

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  (Buf + Pos)

api char *nif_get_string(NIF nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr->strings[i];
}

api char *nif_get_block_type(NIF nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr->block_types[Hedr->block_type_index[i]];
}

api void *nif_get_block(NIF nif, int i) {
	if (i == -1)
	return NULL;
	return Blocks[i];
}

api NIF calloc_nifp() {
	NIF nif = calloc(1, sizeof(Nif));
	nif->hdr = calloc(1, sizeof(struct NifHeader));
	return nif;
}

api void free_nifp(NIF *p) {
	NIF nif = *p;
	free(nif);
	*p = NULL;
}

api void nif_read(NIF nif) {
	assertm(Buf, "nif buf is null ??");
	nif_read_header(nif);
	nif_read_blocks(nif);
}

static void read_short_string(NIF nif, char **string) {
	// has \0
	char len = *(char *)Depos;
	Pos += sizeof(char);
	*string = Depos;
	Pos += len;
}

static void read_sized_string(NIF nif, char **string) {
	// doesnt have \0
	unsigned int len = *(unsigned int *)Depos;
	Pos += sizeof(unsigned int);
	*string = malloc(sizeof(char) * len + 1);
	strncpy(*string, Depos, len);
	(*string)[len] = '\0';
	Pos += len;
}

// begin hedr

static void hedr_read_header_string(NIF nif) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(Buf, '\n') - Buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, Buf, n);
	string[n - 1] = '\0';
	Hedr->header_string = string;
	Hedr->version = string + 30;
	Pos += n;
}

static void read_mem(NIF nif, void *dest, int size) {
	memcpy(dest, Depos, size);
	Pos += size;
}

static void read_array(NIF nif, void **dest, int size) {
	*dest = malloc(size);
	read_mem(nif, *dest, size);
}

static void read_sized_strings(NIF nif, char ***dest, int count) {
	*dest = malloc(count * sizeof(char *));
	for (int i = 0; i < count; i++)
	read_sized_string(nif, &(*dest)[i]);
}

void nif_read_header(NIF nif) {
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

#define DECLARE(x) void *read_ ## x (NIF , int);

static void big_block_reader(NIF , int);

DECLARE( ni_node )
DECLARE( ni_common_layout )
DECLARE( ni_skin_instance )
DECLARE( ni_skin_data )
DECLARE( ni_skin_partition )
DECLARE( bs_tri_shape )
DECLARE( bs_lighting_shader_property )
DECLARE( bs_effect_shader_property )
DECLARE( bs_effect_shader_property_float_controller )
DECLARE( ni_float_interpolator )
DECLARE( ni_float_data )
DECLARE( bs_shader_texture_set )
DECLARE( ni_alpha_property )
DECLARE( ni_controller_sequence )
DECLARE( ni_transform_interpolator )
DECLARE( ni_transform_data )

void nif_read_blocks(NIF nif)
{
	// printf("nifp path %s\n", nif->path);
	unsigned int pos = Pos;
	Blocks = calloc(Hedr->num_blocks, sizeof(void *));
	for (unsigned int i = 0; i < Hedr->num_blocks; i++)
	{
	Blocks[i] = NULL;
	// printf("nifp block begin at %i %04x\n", Pos, Pos);
	big_block_reader(nif, i);
	pos += Hedr->block_sizes[i];
	Pos = pos;
	}
}

#define READ(x) block = read_ ## x(nif, n);

void big_block_reader(NIF nif, int n)
{
	const char *block_type = Hedr->block_types[Hedr->block_type_index[n]];
	void *block = NULL;
	if (0) 0;
	else if ( nif_types(NiNodeS, BSLeafAnimNodeS, BSFadeNodeS) )        READ( ni_node )
	else if ( nif_types(NiSkinInstanceS, BSDismemberSkinInstanceS, 0) ) READ( ni_skin_instance )
	else if ( nif_type(NiSkinDataS) )                                   READ( ni_skin_data )
	else if ( nif_type(NiSkinPartitionS) )                              READ( ni_skin_partition )
	else if ( nif_type(BSDynamicTriShapeS) ) ;
	else if ( nif_type(NiAlphaPropertyS) )                              READ( ni_alpha_property )
	else if ( nif_type(BSTriShapeS) )                                   READ( bs_tri_shape )
	else if ( nif_type(BSLightingShaderPropertyS) )                     READ( bs_lighting_shader_property )
	else if ( nif_type(BSEffectShaderPropertyS) )                       READ( bs_effect_shader_property )
	else if ( nif_type(BSEffectShaderPropertyFloatControllerS) )        READ( bs_effect_shader_property_float_controller )
	else if ( nif_type(NiFloatInterpolatorS) )                          READ( ni_float_interpolator )
	else if ( nif_type(NiFloatDataS) )                                  READ( ni_float_data )
	else if ( nif_type(BSShaderTextureSetS) )                           READ( bs_shader_texture_set )
	else if ( nif_type(NiControllerSequenceS) )                         READ( ni_controller_sequence )
	else if ( nif_type(NiTextKeyExtraDataS) ) ;
	else if ( nif_type(NiStringExtraDataS) ) ;
	else if ( nif_type(NiTransformInterpolatorS) )                      READ( ni_transform_interpolator )
	else if ( nif_type(NiTransformDataS) )                              READ( ni_transform_data )
	else if ( nif_type(BSDecalPlacementVectorExtraDataS) ) ;
	Blocks[n] = block;
}

static inline void sink(NIF nif, void **dest, int size) {
	*dest = Depos;
	Pos += size;
}

#define SINK( nif, block, member )             sink(nif, &block->member, sizeof *block->member);
#define SAIL( nif, block, member, group, num ) sink(nif, &block->member, sizeof *block->member * block->group->num);

#define SKIP(n) Pos += n;

#define CALLOC(x, y) struct x ## _t *y = calloc(1, sizeof(struct x ## _t));

#define BLOCK(x) void *read_ ## x ( NIF nif, int n ) \
{ \
	CALLOC(x, block)

#define BLOCKED() \
	return block; \
} \

BLOCK ( ni_common_layout )
SINK ( nif, block, F )
SAIL ( nif, block, extra_data_list, F, num_extra_data_list )
SINK ( nif, block, A )
BLOCKED ()

BLOCK( ni_node )
block->common = read_ni_common_layout( nif, n );
SINK ( nif, block, A )
SAIL ( nif, block, children, A, num_children )
SINK ( nif, block, B )
SAIL ( nif, block, effects, B, num_effects )
BLOCKED ()

BLOCK( bs_lighting_shader_property )
SINK ( nif, block, A )
SAIL ( nif, block, extra_data_list, A, num_extra_data_list )
SINK ( nif, block, B )
BLOCKED ()

BLOCK( bs_effect_shader_property )
SINK ( nif, block, A )
SAIL ( nif, block, extra_data_list, A, num_extra_data_list )
SINK ( nif, block, B )
read_sized_string( nif, &block->source_texture );
SINK ( nif, block, C )
SINK ( nif, block, falloff )
SINK ( nif, block, D )
read_sized_string( nif, &block->greyscale_texture );
block->meta.u = 0;
block->meta.v = 0;
BLOCKED ()

BLOCK( bs_effect_shader_property_float_controller )
SINK ( nif, block, A )
block->meta.time = 0;
BLOCKED ()

BLOCK( ni_float_interpolator )
SINK ( nif, block, A )
BLOCKED ()

BLOCK( ni_float_data )
SINK ( nif, block, A )
if ( block->A->key_type == 1 )
SAIL ( nif, block, linear_keys, A, num_keys )
if ( block->A->key_type == 2 )
SAIL ( nif, block, quadratic_keys, A, num_keys )
BLOCKED ()

BLOCK( bs_shader_texture_set )
SINK ( nif, block, A )
read_sized_strings( nif, &block->textures, block->A->num_textures );
BLOCKED ()

BLOCK(ni_alpha_property)
SINK ( nif, block, A )
SAIL ( nif, block, extra_data_list, A, num_extra_data_list )
SINK ( nif, block, C )
BLOCKED ()

BLOCK( ni_controller_sequence )
SINK ( nif, block, A )
SAIL ( nif, block, controlled_blocks, A, num_controlled_blocks )
SINK ( nif, block, C )
BLOCKED ()

BLOCK( ni_transform_interpolator )
//printf("read_ni_transform_interpolator\n");
SINK ( nif, block, transform )
SINK ( nif, block, B )
BLOCKED ()

BLOCK( ni_transform_data )
//printf("NiTransformData\n");
SINK ( nif, block, A )

if ( block->A->num_rotation_keys > 0 )
{
	SINK ( nif, block, B )

	if (block->B->rotation_type == 2)
		SAIL ( nif, block, quaternion_keys, A, num_rotation_keys )
	else
		return NULL;
}
SINK ( nif, block, translations )
SAIL ( nif, block, translation_keys, translations, num_keys )
SINK ( nif, block, scales )
SAIL ( nif, block, scale_keys, scales, num_keys )
BLOCKED ()

BLOCK( ni_skin_instance )
SINK ( nif, block, A )
SAIL ( nif, block, bones, A, num_bones )
char *block_type = nif_get_block_type( nif, n );
if (nif_type(BSDismemberSkinInstanceS))
{
	SINK ( nif, block, B )
	SAIL ( nif, block, partitions, B, num_partitions )
}
BLOCKED ()

#define SUBBLOCK(x, y, group, num) \
	block->x = calloc(block->group->num, sizeof(struct y ## _t *)); \
	for (unsigned int i = 0; i < block->group->num; i++) { \
	block->x[i] = malloc(sizeof(struct y ## _t)); \
	struct y ## _t *y = block->x[i];
	
#define LOOPED() }

BLOCK( ni_skin_data )
SINK ( nif, block, skin_transform )
SINK ( nif, block, A )
SUBBLOCK ( bone_list, bone_data, A, num_bones )
	SINK ( nif, bone_data, skin_transform )
	SINK ( nif, bone_data, bounding_sphere )
	SINK ( nif, bone_data, A )
	SAIL ( nif, bone_data, vertex_weights, A, num_vertices )
LOOPED ()
BLOCKED ()

BLOCK( ni_skin_partition )
SINK ( nif, block, A )
sink( nif, &block->vertex_data, block->A->data_size );
SUBBLOCK ( partitions, skin_partition, A, num_partitions )
	SINK ( nif, skin_partition, nums )
	SAIL ( nif, skin_partition, bones, nums, bones )
	SINK ( nif, skin_partition, has_vertex_map )
	SAIL ( nif, skin_partition, vertex_map, nums, vertices )
	SINK ( nif, skin_partition, has_vertex_weights )
	SAIL ( nif, skin_partition, vertex_weights, nums, vertices )
	SAIL ( nif, skin_partition, strip_lengths, nums, strips )
	SINK ( nif, skin_partition, has_faces )
	SAIL ( nif, skin_partition, triangles, nums, triangles )
	SINK ( nif, skin_partition, has_bone_indices )
	SAIL ( nif, skin_partition, bone_indices, nums, vertices )
	SINK ( nif, skin_partition, F )
	SAIL ( nif, skin_partition, triangles_copy, nums, triangles )
LOOPED ()
BLOCKED ()

BLOCK ( bs_tri_shape )
block->common = read_ni_common_layout( nif, n );
SINK ( nif, block, bounding_sphere )
SINK ( nif, block, refs )
SINK ( nif, block, infos )
int vertex, uvs, normals, tangents, colors, skinned;
nif_bs_vertex_desc(block->infos->vertex_desc, &vertex, &uvs, &normals, &tangents, &colors, &skinned);
// all models use these two variants it seems
if ( vertex && uvs && normals && tangents && colors )
	SAIL ( nif, block, vertex_data_all, infos, num_vertices)

else if ( vertex && uvs && normals && tangents && !colors )
	SAIL ( nif, block, vertex_data_no_clr, infos, num_vertices)
else {
	// xmarkerx
	// marker_prison
	// marker cocheading
	// printf("\nsse for %s has %i %i %i %i %i %i\n", nif->path, vertex, uvs, normals, tangents, colors);
	SKIP ( block->infos->data_size )
}
SAIL ( nif, block, triangles, infos, num_triangles )
SINK ( nif, block, particle_data_size )
BLOCKED ()