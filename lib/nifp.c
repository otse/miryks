/// nif with pointers

// will read, sort legendary and special edition nifs

// has its own block reading language called sink, sail
// :)

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
#define Depos  (Buf + Pos)

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
	char len = *(char *)Depos;
	Pos += sizeof(char);
	*string = Depos;
	Pos += len;
}

static void read_sized_string(Nifp *nif, char **string) {
	// doesnt have \0
	unsigned int len = *(unsigned int *)Depos;
	Pos += sizeof(unsigned int);
	*string = malloc(sizeof(char) * len + 1);
	strncpy(*string, Depos, len);
	(*string)[len] = '\0';
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

void nifp_read_header(Nifp *nif) {
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

// todo get RID of these function declarations somehow
static void big_block_reader(Nifp *, int);

legendary_edition
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
special_edition
static void *read_bs_tri_shape(nifpr);

void nifp_read_blocks(Nifp *nif)
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

void big_block_reader(Nifp *nif, int n)
{
	const char *block_type = Hedr->block_types[Hedr->block_type_index[n]];
	void *block = NULL;
	if (0) ;
	else if ( ni_is_any(NI_NODE, BS_LEAF_ANIM_NODE, BS_FADE_NODE) ) block = read_ni_node(nif, n);
	else if ( ni_is_any(NI_SKIN_INSTANCE, BS_DISMEMBER_SKIN_INSTANCE, NULL) ) block = read_ni_skin_instance(nif, n);
	else if ( ni_is_type(NI_SKIN_DATA) ) block = read_ni_skin_data(nif, n);
	else if ( ni_is_type(NI_SKIN_PARTITION) ) block = read_ni_skin_partition(nif, n);
	special_edition
	else if ( ni_is_type(BS_TRI_SHAPE) ) block = read_bs_tri_shape(nif, n);
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

// sink, sail

static inline void sink(Nifp *nif, void **dest, int size) {
	*dest = Depos;
	Pos += size;
}

#define SINK( nif, block, part ) \
	sink(nif, &block->part, sizeof *block->part);

#define SAIL( nif, block, part, group, num ) \
	sink(nif, &block->part, sizeof *block->part * block->group->num);

#define SKIP(n) Pos += n;

#define CALLOC( type ) type *block = calloc(1, sizeof(type));

#define NI_READ(x) \
void *read_ ## x ( Nifp *nif, int n ) \
{ \
	CALLOC(struct x ## _pointer)

#define END_READ() \
	return block; \
} \

NI_READ ( ni_common_layout )

SINK ( nif, block, F )
SAIL ( nif, block, extra_data_list, F, num_extra_data_list )
SINK ( nif, block, A )

END_READ()

NI_READ( ni_node )

block->common = read_ni_common_layout( nif, n );
SINK ( nif, block, A )
SAIL ( nif, block, children, A, num_children )
SINK ( nif, block, B )
SAIL ( nif, block, effects, B, num_effects )

END_READ()

NI_READ( bs_lighting_shader_property )

SINK ( nif, block, A )
SAIL ( nif, block, extra_data_list, A, num_extra_data_list )
SINK ( nif, block, B )

END_READ()

NI_READ( bs_effect_shader_property )

SINK ( nif, block, A )
SAIL ( nif, block, extra_data_list, A, num_extra_data_list )
SINK ( nif, block, B )
read_sized_string( nif, &block->source_texture );
SINK ( nif, block, C )
read_sized_string( nif, &block->greyscale_texture );

END_READ()

NI_READ( bs_shader_texture_set )

SINK ( nif, block, A )
read_sized_strings( nif, &block->textures, block->A->num_textures );

END_READ()

NI_READ(ni_alpha_property)

SINK ( nif, block, A )
SAIL ( nif, block, extra_data_list, A, num_extra_data_list )
SINK ( nif, block, C )

END_READ()

NI_READ( ni_controller_sequence )

SINK ( nif, block, A )
SAIL ( nif, block, controlled_blocks, A, num_controlled_blocks )
SINK ( nif, block, C )

END_READ()

NI_READ( ni_transform_interpolator )

//printf("read_ni_transform_interpolator\n");
SINK ( nif, block, transform )
SINK ( nif, block, B )

END_READ()

NI_READ( ni_transform_data ) legendary_edition

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

END_READ()

NI_READ( ni_tri_shape ) legendary_edition

printf("read ni tri shape pointer!\n");
block->common = read_ni_common_layout( nif, n );
SINK ( nif, block, A )
SKIP( 9 )
SINK ( nif, block, B )

END_READ()

NI_READ( ni_tri_shape_data ) legendary_edition

printf(" ni tri shape data in sse ?!\n");

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

END_READ()

NI_READ( ni_skin_instance ) legendary_edition

SINK ( nif, block, A )
SAIL ( nif, block, bones, A, num_bones )

char *block_type = nifp_get_block_type( nif, n );
if (ni_is_type(BS_DISMEMBER_SKIN_INSTANCE))
{
	SINK ( nif, block, B )
	SAIL ( nif, block, partitions, B, num_partitions )
}

END_READ()

NI_READ( ni_skin_data ) legendary_edition

//printf("read ni skin data\n");
SINK ( nif, block, skin_transform )
SINK ( nif, block, B )

// todo build a macro to do this dirty part

block->bone_list = malloc(sizeof(struct bone_data *) * block->B->num_bones);
for (unsigned int i = 0; i < block->B->num_bones; i++)
{
	block->bone_list[i] = malloc(sizeof(struct bone_data));
	struct bone_data *bone_data = block->bone_list[i];
	SINK ( nif, bone_data, skin_transform )
	SINK ( nif, bone_data, B )
	SAIL ( nif, bone_data, vertex_weights, B, num_vertices )
}
END_READ()

NI_READ( ni_skin_partition ) legendary_edition

SINK ( nif, block, num_skin_partition_blocks )

block->skin_partition_blocks = calloc(*block->num_skin_partition_blocks, sizeof(struct skin_partition *));
for (unsigned int i = 0; i < *block->num_skin_partition_blocks; i++)
{
	block->skin_partition_blocks[i] = calloc(1, sizeof(struct skin_partition));
	struct skin_partition *skin_partition = block->skin_partition_blocks[i];

	SINK ( nif, skin_partition, A )
	SAIL ( nif, skin_partition, bones, A, num_bones )
	SINK ( nif, skin_partition, has_vertex_map )

	if (*skin_partition->has_vertex_map)
		SAIL ( nif, skin_partition, vertex_map, A, num_vertices )

	SINK ( nif, skin_partition, has_vertex_weights )

	if (*skin_partition->has_vertex_weights)
		SAIL ( nif, skin_partition, vertex_weights, A, num_vertices )

	SAIL ( nif, skin_partition, strip_lengths, A, num_strips )

	SINK ( nif, skin_partition, has_faces )

	if (*skin_partition->has_faces)
		SAIL ( nif, skin_partition, triangles, A, num_triangles )

	SINK ( nif, skin_partition, has_bone_indices )
	
	if (*skin_partition->has_bone_indices)
		SAIL ( nif, skin_partition, bone_indices, A, num_vertices )

	SINK ( nif, skin_partition, unknown_short )
}

END_READ()

NI_READ ( bs_tri_shape ) special_edition

block->common = read_ni_common_layout( nif, n );

SINK ( nif, block, bounding_sphere )
SINK ( nif, block, refs )
SINK ( nif, block, infos )

int vertex, uvs, normals, tangents, colors, skinned;
nifp_sse_dissect_vertex_desc(block->infos->vertex_desc, &vertex, &uvs, &normals, &tangents, &colors, &skinned);

// all models use these two variants it seems
if ( vertex && uvs && normals && tangents && colors )
	SAIL ( nif, block, vertex_data_all, infos, num_vertices)

else if ( vertex && uvs && normals && tangents && !colors )
	SAIL ( nif, block, vertex_data_no_clr, infos, num_vertices)

else {
	// xmarkerx
	// marker_prison
	// marker cocheading
	printf("\nsse for %s has %i %i %i %i %i %i\n", nif->path, vertex, uvs, normals, tangents, colors);
}

SAIL ( nif, block, triangles, infos, num_triangles )
SINK ( nif, block, particle_data_size )

END_READ()