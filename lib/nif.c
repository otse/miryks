/// nif.c

#include "common.h"

#include "nif.h"
#include "niftypes.h"

#define Hedr   nif->hdr
#define Buf    nif->buf
#define Pos    nif->pos
#define Blocks nif->blocks
#define Depos  (Buf + Pos)

api char *nif_get_string(Nif *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr->strings[i];
}

api char *nif_get_block_type(Nif *nif, int i) {
	if (i == -1)
	return NULL;
	return Hedr->block_types[Hedr->block_type_index[i]];
}

api void *nif_get_block(Nif *nif, int i) {
	if (i == -1)
	return NULL;
	return Blocks[i];
}

api Nif *calloc_nifp() {
	Nif *nif = calloc(1, sizeof(Nif));
	nif->hdr = calloc(1, sizeof(struct NifHeader));
	return nif;
}

api void free_nifp(Nif **p) {
	Nif *nif = *p;
	free(nif);
	*p = NULL;
}

api void nif_read(Nif *nif) {
	assertm(Buf, "nif buf is null ??");
	nif_read_header(nif);
	nif_read_blocks(nif);
}

static void read_short_string(Nif *nif, char **string) {
	// has \0
	char len = *(char *)Depos;
	Pos += sizeof(char);
	*string = Depos;
	Pos += len;
}

static void read_sized_string(Nif *nif, char **string) {
	// doesnt have \0
	unsigned int len = *(unsigned int *)Depos;
	Pos += sizeof(unsigned int);
	*string = malloc(sizeof(char) * len + 1);
	strncpy(*string, Depos, len);
	(*string)[len] = '\0';
	Pos += len;
}

// begin hedr

static void hedr_read_header_string(Nif *nif) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(Buf, '\n') - Buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, Buf, n);
	string[n - 1] = '\0';
	Hedr->header_string = string;
	Hedr->version = string + 30;
	Pos += n;
}

static void read_mem(Nif *nif, void *dest, int size) {
	memcpy(dest, Depos, size);
	Pos += size;
}

static void read_array(Nif *nif, void **dest, int size) {
	*dest = malloc(size);
	read_mem(nif, *dest, size);
}

static void read_sized_strings(Nif *nif, char ***dest, int count) {
	*dest = malloc(count * sizeof(char *));
	for (int i = 0; i < count; i++)
	read_sized_string(nif, &(*dest)[i]);
}

void nif_read_header(Nif *nif) {
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


#define DECLARE(x) void *read_ ## x (Nif *, int);

static void big_block_reader(Nif *, int);

DECLARE( ni_node )
DECLARE( ni_common_layout )
DECLARE( ni_skin_instance )
DECLARE( ni_skin_data )
DECLARE( ni_skin_partition )
DECLARE( bs_tri_shape )
#ifdef SLE
DECLARE( ni_tri_shape )
DECLARE( ni_tri_shape_data )
#endif
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

void nif_read_blocks(Nif *nif)
{
	// printf("nifp path %s\n", nif->path);
	unsigned int pos = Pos;
	Blocks = malloc(sizeof(void *) * Hedr->num_blocks);
	for (unsigned int i = 0; i < Hedr->num_blocks; i++)
	{
	Blocks[i] = NULL;
	// printf("nifp block begin at %i %04x\n", Pos, Pos);
	big_block_reader(nif, i);
	pos += Hedr->block_sizes[i];
	Pos = pos;
	}
}

#define READ(x) block = read_ ## x(nif, n)

void big_block_reader(Nif *nif, int n)
{
	const char *block_type = Hedr->block_types[Hedr->block_type_index[n]];
	void *block = NULL;
	if (0) 0;
	else if ( nif_types(NiNodeS, BSLeafAnimNodeS, BSFadeNodeS) )        READ( ni_node );
	else if ( nif_types(NiSkinInstanceS, BSDismemberSkinInstanceS, 0) ) READ( ni_skin_instance );
	else if ( nif_type(NiSkinDataS) )                                   READ( ni_skin_data );
	else if ( nif_type(NiSkinPartitionS) )                              READ( ni_skin_partition );
	else if ( nif_type(BSDynamicTriShapeS) ) ;
	else if ( nif_type(NiAlphaPropertyS) )                              READ( ni_alpha_property );
	#ifdef SLE
	else if ( nif_types(NiTriShapeS, BSLODTriShapeS, 0) )               READ( ni_tri_shape );
	else if ( nif_type(NiTriShapeDataS) )                               READ( ni_tri_shape_data );
	#endif
	#ifdef SSE
	else if ( nif_type(BSTriShapeS) )                                   READ( bs_tri_shape );
	#endif
	else if ( nif_type(BSLightingShaderPropertyS) )                     READ( bs_lighting_shader_property );
	else if ( nif_type(BSEffectShaderPropertyS) )                       READ( bs_effect_shader_property );
	else if ( nif_type(BSEffectShaderPropertyFloatControllerS) )        READ( bs_effect_shader_property_float_controller );
	else if ( nif_type(NiFloatInterpolatorS) )                          READ( ni_float_interpolator );
	else if ( nif_type(NiFloatDataS) )                                  READ( ni_float_data );
	else if ( nif_type(BSShaderTextureSetS) )                           READ( bs_shader_texture_set );
	else if ( nif_type(NiControllerSequenceS) )                         READ( ni_controller_sequence );
	else if ( nif_type(NiTextKeyExtraDataS) ) ;
	else if ( nif_type(NiStringExtraDataS) ) ;
	else if ( nif_type(NiTransformInterpolatorS) )                      READ( ni_transform_interpolator ); 
	else if ( nif_type(NiTransformDataS) )                              READ( ni_transform_data );
	else if ( nif_type(BSDecalPlacementVectorExtraDataS) ) ;
	Blocks[n] = block;
}


static inline void sink(Nif *nif, void **dest, int size) {
	*dest = Depos;
	Pos += size;
}

#define SINK( nif, block, member )             sink(nif, &block->member, sizeof *block->member);
#define SAIL( nif, block, member, group, num ) sink(nif, &block->member, sizeof *block->member * block->group->num);

#define SKIP(n) Pos += n;

#define CALLOC( type ) type *block = calloc(1, sizeof(type));

#define BEGIN(x) void *read_ ## x ( Nif *nif, int n ) \
{ \
	CALLOC(struct x ## _t)

#define END() \
	return block; \
} \

BEGIN ( ni_common_layout )
SINK ( nif, block, F )
SAIL ( nif, block, extra_data_list, F, num_extra_data_list )
SINK ( nif, block, A )
END ()

BEGIN( ni_node )
block->common = read_ni_common_layout( nif, n );
SINK ( nif, block, A )
SAIL ( nif, block, children, A, num_children )
SINK ( nif, block, B )
SAIL ( nif, block, effects, B, num_effects )
END ()

BEGIN( bs_lighting_shader_property )
SINK ( nif, block, A )
SAIL ( nif, block, extra_data_list, A, num_extra_data_list )
SINK ( nif, block, B )
END ()

BEGIN( bs_effect_shader_property )
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
END ()

BEGIN( bs_effect_shader_property_float_controller )
SINK ( nif, block, A )
block->meta.time = 0;
END ()

BEGIN( ni_float_interpolator )
SINK ( nif, block, A )
END ()

BEGIN( ni_float_data )
SINK ( nif, block, A )
if ( block->A->key_type == 1 )
SAIL ( nif, block, linear_keys, A, num_keys )
if ( block->A->key_type == 2 )
SAIL ( nif, block, quadratic_keys, A, num_keys )
END ()

BEGIN( bs_shader_texture_set )
SINK ( nif, block, A )
read_sized_strings( nif, &block->textures, block->A->num_textures );
END ()

BEGIN(ni_alpha_property)
SINK ( nif, block, A )
SAIL ( nif, block, extra_data_list, A, num_extra_data_list )
SINK ( nif, block, C )
END ()

BEGIN( ni_controller_sequence )
SINK ( nif, block, A )
SAIL ( nif, block, controlled_blocks, A, num_controlled_blocks )
SINK ( nif, block, C )
END ()

BEGIN( ni_transform_interpolator )
//printf("read_ni_transform_interpolator\n");
SINK ( nif, block, transform )
SINK ( nif, block, B )
END ()

BEGIN( ni_transform_data )
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
END ()

#ifdef SLE

BEGIN( ni_tri_shape )
printf("read NiTriShape\n");

block->common = read_ni_common_layout( nif, n );
SINK ( nif, block, A )
SKIP( 9 )
SINK ( nif, block, B )

END ()

BEGIN( ni_tri_shape_data )
printf("read NiTriShapeData\n");

SINK ( nif, block, A )

if ( block->A->has_vertices )
	SAIL ( nif, block, vertices, A, num_vertices )

SINK ( nif, block, C )

if ( block->C->has_normals )
	SAIL ( nif, block, normals, A, num_vertices )

if ( block->C->bs_vector_flags & 0x00001000 )
{
	SAIL ( nif, block, tangents, A, num_vertices )
	SAIL ( nif, block, bitangents, A, num_vertices )
}

SINK ( nif, block, G )

if ( block->G->has_vertex_colors )
	SAIL ( nif, block, vertex_colors, A, num_vertices )

if ( block->C->bs_vector_flags & 0x00000001 )
	SAIL ( nif, block, uv_sets, A, num_vertices )

SINK ( nif, block, J )

if (block->J->has_triangles)
	SAIL ( nif, block, triangles, J, num_triangles )

SINK ( nif, block, L )

SAIL ( nif, block, match_groups, L, num_match_groups )

END ()

#endif

BEGIN( ni_skin_instance )
SINK ( nif, block, A )
SAIL ( nif, block, bones, A, num_bones )
char *block_type = nif_get_block_type( nif, n );
if (nif_type(BSDismemberSkinInstanceS))
{
	SINK ( nif, block, B )
	SAIL ( nif, block, partitions, B, num_partitions )
}
END ()

BEGIN( ni_skin_data )
//printf("read ni skin data\n");
SINK ( nif, block, skin_transform )
SINK ( nif, block, B )
// todo build a macro to do this dirty part
block->bone_list = malloc(sizeof(struct bone_data_t *) * block->B->num_bones);
for (unsigned int i = 0; i < block->B->num_bones; i++)
{
	block->bone_list[i] = malloc(sizeof(struct bone_data_t));
	struct bone_data_t *bone_data = block->bone_list[i];
	SINK ( nif, bone_data, skin_transform )
	SINK ( nif, bone_data, B )
	SAIL ( nif, bone_data, vertex_weights, B, num_vertices )
}
END ()

BEGIN( ni_skin_partition )
SINK ( nif, block, num_skin_partition_blocks )

END ()

BEGIN ( bs_tri_shape )
block->common = read_ni_common_layout( nif, n );

SINK ( nif, block, bounding_sphere )
SINK ( nif, block, refs )
SINK ( nif, block, infos )

int vertex, uvs, normals, tangents, colors, skinned;
nif_sse_dissect_vertex_desc(block->infos->vertex_desc, &vertex, &uvs, &normals, &tangents, &colors, &skinned);

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
	return block;
}

SAIL ( nif, block, triangles, infos, num_triangles )
SINK ( nif, block, particle_data_size )
END ()