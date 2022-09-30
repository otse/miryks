/// nif.c

#include <lib/common.h>

#include <lib/nif.h>
#include <lib/niftypes.h>

#define Hedr   ni->hdr
#define Buf    ni->buf
#define Pos    ni->pos
#define Blocks ni->blocks
#define Depos  (Buf + Pos)

api char *nif_get_string(NIF ni, int i) {
	if (i == -1)
	return NULL;
	return Hedr->strings[i];
}

api char *nif_get_block_type(NIF ni, int i) {
	if (i == -1)
	return NULL;
	return Hedr->block_types[Hedr->block_type_index[i]];
}

api void *nif_get_block(NIF ni, int i) {
	if (i == -1)
	return NULL;
	return Blocks[i];
}

api NIF calloc_ni() {
	NIF ni = calloc(1, sizeof(nif));
	ni->hdr = calloc(1, sizeof(struct NifHeader));
	return ni;
}

api void free_ni(NIF *p) {
	NIF ni = *p;
	free(ni);
	*p = NULL;
}

api void nif_read(NIF ni) {
	assertm(Buf, "ni buf is null ??");
	nif_read_header(ni);
	nif_read_blocks(ni);
}

static void read_short_string(NIF ni, char **string) {
	// has \0
	char len = *(char *)Depos;
	Pos += sizeof(char);
	*string = Depos;
	Pos += len;
}

static void read_sized_string(NIF ni, char **string) {
	// doesnt have \0
	unsigned int len = *(unsigned int *)Depos;
	Pos += sizeof(unsigned int);
	*string = malloc(sizeof(char) * len + 1);
	strncpy(*string, Depos, len);
	(*string)[len] = '\0';
	Pos += len;
}

// begin hedr

static void hedr_read_header_string(NIF ni) {
	// Gamebryo File Format, Version 20.2.0.7\n
	int n = strchr(Buf, '\n') - Buf + 1;
	char *string = malloc(sizeof(char) * n);
	strncpy(string, Buf, n);
	string[n - 1] = '\0';
	Hedr->header_string = string;
	Hedr->version = string + 30;
	Pos += n;
}

static void read_mem(NIF ni, void *dest, int size) {
	memcpy(dest, Depos, size);
	Pos += size;
}

static void read_array(NIF ni, void **dest, int size) {
	*dest = malloc(size);
	read_mem(ni, *dest, size);
}

static void read_sized_strings(NIF ni, char ***dest, int count) {
	*dest = malloc(count * sizeof(char *));
	for (int i = 0; i < count; i++)
	read_sized_string(ni, &(*dest)[i]);
}

void nif_read_header(NIF ni) {
	hedr_read_header_string(ni);
	read_mem(ni, &Hedr->unknown_1, 17);
	read_short_string(ni, &Hedr->author);
	read_short_string(ni, &Hedr->process_script);
	read_short_string(ni, &Hedr->export_script);
	read_mem(ni, &Hedr->num_block_types, 2);
	read_sized_strings(ni, &Hedr->block_types, Hedr->num_block_types);
	read_array(ni, &Hedr->block_type_index, sizeof(unsigned short) * Hedr->num_blocks);
	read_array(ni, &Hedr->block_sizes,      sizeof(unsigned int)   * Hedr->num_blocks);
	read_mem(ni, &Hedr->num_strings, 8);
	read_sized_strings(ni, &Hedr->strings, Hedr->num_strings);
	read_mem(ni, &Hedr->num_groups, 4);
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
DECLARE( bs_dynamic_tri_shape )
DECLARE( bs_lighting_shader_property )
DECLARE( bs_water_shader_property )
DECLARE( bs_effect_shader_property )
DECLARE( bs_effect_shader_property_float_controller )
DECLARE( ni_float_interpolator )
DECLARE( ni_float_data )
DECLARE( bs_shader_texture_set )
DECLARE( ni_alpha_property )
DECLARE( ni_controller_sequence )
DECLARE( ni_transform_interpolator )
DECLARE( ni_transform_data )

void nif_read_blocks(NIF ni)
{
	// printf("nifp path %s\n", ni->path);
	unsigned int pos = Pos;
	Blocks = calloc(Hedr->num_blocks, sizeof(void *));
	for (unsigned int i = 0; i < Hedr->num_blocks; i++)
	{
	Blocks[i] = NULL;
	// printf("nifp block begin at %i %04x\n", Pos, Pos);
	big_block_reader(ni, i);
	pos += Hedr->block_sizes[i];
	Pos = pos;
	}
}

#define READ(x) block = read_ ## x(ni, n);

void big_block_reader(NIF ni, int n)
{
	const char *block_type = Hedr->block_types[Hedr->block_type_index[n]];
	void *block = NULL;
	if (0) 0;
	else if ( nif_types(NiNodeS, BSLeafAnimNodeS, BSFadeNodeS, BSMultiBoundNodeS) ) READ( ni_node )
	else if ( nif_types(NiSkinInstanceS, BSDismemberSkinInstanceS, 0, 0) )          READ( ni_skin_instance )
	else if ( nif_type(NiSkinDataS) )                                               READ( ni_skin_data )
	else if ( nif_type(NiSkinPartitionS) )                                          READ( ni_skin_partition )
	else if ( nif_type(NiAlphaPropertyS) )                                          READ( ni_alpha_property )
	else if ( nif_type(BSTriShapeS) )                                               READ( bs_tri_shape )
	else if ( nif_type(BSDynamicTriShapeS) )                                        READ( bs_dynamic_tri_shape )
	else if ( nif_type(BSLightingShaderPropertyS) )                                 READ( bs_lighting_shader_property )
	else if ( nif_type(BSWaterShaderPropertyS) )                     	            READ( bs_water_shader_property )
	else if ( nif_type(BSEffectShaderPropertyS) )                                   READ( bs_effect_shader_property )
	else if ( nif_type(BSEffectShaderPropertyFloatControllerS) )                    READ( bs_effect_shader_property_float_controller )
	else if ( nif_type(NiFloatInterpolatorS) )                                      READ( ni_float_interpolator )
	else if ( nif_type(NiFloatDataS) )                                              READ( ni_float_data )
	else if ( nif_type(BSShaderTextureSetS) )                                       READ( bs_shader_texture_set )
	else if ( nif_type(NiControllerSequenceS) )                                     READ( ni_controller_sequence )
	else if ( nif_type(NiTextKeyExtraDataS) ) ;
	else if ( nif_type(NiStringExtraDataS) ) ;
	else if ( nif_type(NiTransformInterpolatorS) )                                  READ( ni_transform_interpolator )
	else if ( nif_type(NiTransformDataS) )                                          READ( ni_transform_data )
	else if ( nif_type(BSDecalPlacementVectorExtraDataS) ) ;
	Blocks[n] = block;
}

static inline void sink(NIF ni, void **dest, int size) {
	*dest = Depos;
	Pos += size;
}

#define SINK( ni, block, member )             sink(ni, &block->member, sizeof *block->member);
#define SAIL( ni, block, member, group, num ) sink(ni, &block->member, sizeof *block->member * block->group->num);

#define SKIP(n) Pos += n;

#define CALLOC(x, y) struct x ## _t *y = calloc(1, sizeof(struct x ## _t));

#define BLOCK(x) void *read_ ## x ( NIF ni, int n ) \
{ \
	CALLOC(x, block)

#define BLOCKED() \
	return block; \
} \

BLOCK ( ni_common_layout )
SINK ( ni, block, F )
SAIL ( ni, block, extra_data_list, F, num_extra_data_list )
SINK ( ni, block, A )
BLOCKED ()

BLOCK( ni_node )
block->common = read_ni_common_layout( ni, n );
SINK ( ni, block, A )
SAIL ( ni, block, children, A, num_children )
SINK ( ni, block, B )
SAIL ( ni, block, effects, B, num_effects )
BLOCKED ()

BLOCK( bs_lighting_shader_property )
SINK ( ni, block, A )
SAIL ( ni, block, extra_data_list, A, num_extra_data_list )
SINK ( ni, block, B )
BLOCKED ()

BLOCK( bs_water_shader_property )
SINK ( ni, block, A )
SAIL ( ni, block, extra_data_list, A, num_extra_data_list )
SINK ( ni, block, B )
BLOCKED ()

BLOCK( bs_effect_shader_property )
SINK ( ni, block, A )
SAIL ( ni, block, extra_data_list, A, num_extra_data_list )
SINK ( ni, block, B )
read_sized_string( ni, &block->source_texture );
SINK ( ni, block, C )
SINK ( ni, block, falloff )
SINK ( ni, block, D )
read_sized_string( ni, &block->greyscale_texture );
block->meta.u = 0;
block->meta.v = 0;
BLOCKED ()

BLOCK( bs_effect_shader_property_float_controller )
SINK ( ni, block, A )
block->meta.time = 0;
BLOCKED ()

BLOCK( ni_float_interpolator )
SINK ( ni, block, A )
BLOCKED ()

BLOCK( ni_float_data )
SINK ( ni, block, A )
if ( block->A->key_type == 1 )
SAIL ( ni, block, linear_keys, A, num_keys )
if ( block->A->key_type == 2 )
SAIL ( ni, block, quadratic_keys, A, num_keys )
BLOCKED ()

BLOCK( bs_shader_texture_set )
SINK ( ni, block, A )
read_sized_strings( ni, &block->textures, block->A->num_textures );
BLOCKED ()

BLOCK(ni_alpha_property)
SINK ( ni, block, A )
SAIL ( ni, block, extra_data_list, A, num_extra_data_list )
SINK ( ni, block, C )
BLOCKED ()

BLOCK( ni_controller_sequence )
SINK ( ni, block, A )
SAIL ( ni, block, controlled_blocks, A, num_controlled_blocks )
SINK ( ni, block, C )
BLOCKED ()

BLOCK( ni_transform_interpolator )
//printf("read_ni_transform_interpolator\n");
SINK ( ni, block, transform )
SINK ( ni, block, B )
BLOCKED ()

BLOCK( ni_transform_data )
//printf("NiTransformData\n");
SINK ( ni, block, A )

if ( block->A->num_rotation_keys > 0 )
{
	SINK ( ni, block, B )

	if (block->B->rotation_type == 2)
		SAIL ( ni, block, quaternion_keys, A, num_rotation_keys )
	else
		return NULL;
}
// why do we return null
SINK ( ni, block, translations )
SAIL ( ni, block, translation_keys, translations, num_keys )
SINK ( ni, block, scales )
SAIL ( ni, block, scale_keys, scales, num_keys )
BLOCKED ()

BLOCK( ni_skin_instance )
SINK ( ni, block, A )
SAIL ( ni, block, bones, A, num_bones )
char *block_type = nif_get_block_type( ni, n );
if (nif_type(BSDismemberSkinInstanceS))
{
	SINK ( ni, block, B )
	SAIL ( ni, block, partitions, B, num_partitions )
}
BLOCKED ()

#define SUBBLOCK(x, y, group, num) \
	block->x = calloc(block->group->num, sizeof(struct y ## _t *)); \
	for (unsigned int i = 0; i < block->group->num; i++) { \
	block->x[i] = malloc(sizeof(struct y ## _t)); \
	struct y ## _t *y = block->x[i];
	
#define LOOPED() }

BLOCK( ni_skin_data )
SINK ( ni, block, skin_transform )
SINK ( ni, block, A )
SUBBLOCK ( bone_list, bone_data, A, num_bones )
	SINK ( ni, bone_data, skin_transform )
	SINK ( ni, bone_data, bounding_sphere )
	SINK ( ni, bone_data, A )
	SAIL ( ni, bone_data, vertex_weights, A, num_vertices )
LOOPED ()
BLOCKED ()

BLOCK( ni_skin_partition )
SINK ( ni, block, A )
sink( ni, &block->vertex_data, block->A->data_size );
SUBBLOCK ( partitions, skin_partition, A, num_partitions )
	SINK ( ni, skin_partition, nums )
	SAIL ( ni, skin_partition, bones, nums, bones )
	SINK ( ni, skin_partition, has_vertex_map )
	SAIL ( ni, skin_partition, vertex_map, nums, vertices )
	SINK ( ni, skin_partition, has_vertex_weights )
	SAIL ( ni, skin_partition, vertex_weights, nums, vertices )
	SAIL ( ni, skin_partition, strip_lengths, nums, strips )
	SINK ( ni, skin_partition, has_faces )
	SAIL ( ni, skin_partition, triangles, nums, triangles )
	SINK ( ni, skin_partition, has_bone_indices )
	SAIL ( ni, skin_partition, bone_indices, nums, vertices )
	SINK ( ni, skin_partition, F )
	SAIL ( ni, skin_partition, triangles_copy, nums, triangles )
LOOPED ()
BLOCKED ()

BLOCK ( bs_tri_shape )
block->common = read_ni_common_layout( ni, n );
SINK ( ni, block, bounding_sphere )
SINK ( ni, block, refs )
SINK ( ni, block, infos )
int vertex, uvs, normals, tangents, colors, skinned, eyedata, fullprec;
nif_bs_vertex_desc(
	block->infos->vertex_desc, &vertex, &uvs, &normals, &tangents, &colors, &skinned, &eyedata, &fullprec);
// all models use these two variants it seems
if ( vertex && uvs && normals && tangents && colors )
	SAIL ( ni, block, vertex_data_all, infos, num_vertices)

else if ( vertex && uvs && normals && tangents && !colors )
	SAIL ( ni, block, vertex_data_no_clr, infos, num_vertices)
else {
	// xmarkerx
	// marker_prison
	// marker cocheading
	//printf("\nbs tri shape vertex data %s (num: %i) has %i %i %i %i %i %i\n", ni->path, n, vertex, uvs, normals, tangents, colors);
	SKIP ( block->infos->data_size )
}
SAIL ( ni, block, triangles, infos, num_triangles )
SINK ( ni, block, particle_data_size )
BLOCKED ()

BLOCK ( bs_dynamic_tri_shape )
printf("read bs dynamic tri shape\n");
block->bs_tri_shape = read_bs_tri_shape( ni, n );
SINK ( ni, block, A )
SAIL ( ni, block, vertices, bs_tri_shape->infos, num_vertices )
printf("sail block vertices bs tri shape infos num vertices\n");
//SINK ( ni, block, particle_data_size )
BLOCKED ()