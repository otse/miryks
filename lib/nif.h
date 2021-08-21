#ifndef LIB_NIF_H
#define LIB_NIF_H

/// nif.h

#include "niftypes.h"

#define api

#define special_edition

#define legendary_edition

//#define LSE
#define SSE

#define NiRef int

struct NifHeader;

struct Nif;
struct Rd;

typedef struct Nif Nif;
typedef struct Rd Rd;

struct Nif
{
	int num;
	const char *path;
	const unsigned char *buf;
	unsigned pos;
	struct NifHeader *hdr;
	void **blocks;
};

#define callback(x) void (*x ## _callback) (struct Rd *, struct x ## _t *);

struct Rd
{
	int x;
	char *skips;
	Nif *nif;
	void *data;
	int parent, current;
	void (*other) (struct Rd *, void *block);
	callback(ni_node)
	callback(ni_tri_shape)
	callback(ni_tri_shape_data)
	callback(bs_tri_shape) special_edition
	callback(bs_shader_texture_set)
	callback(ni_alpha_property)
	callback(ni_controller_sequence)
	callback(ni_skin_instance)
	callback(ni_skin_data)
	callback(ni_skin_partition)
	callback(bs_lighting_shader_property)
	callback(bs_effect_shader_property)
	callback(bs_effect_shader_property_float_controller)
	callback(ni_float_interpolator)
	callback(ni_float_data)
};

#undef callback

void nif_test();

void nif_read_header(Nif *);
void nif_read_blocks(Nif *);

api Nif *calloc_nifp();
api Rd  *calloc_nifprd();

api void free_nifp  (Nif **);
api void free_nifprd(Rd **);

api void nif_rd(Rd *);

api void nif_read(Nif *);

api char *nif_get_string(Nif *, int);
api char *nif_get_block_type(Nif *, int);
api void *nif_get_block(Nif *, int);

api void nif_print_hedr(Nif *, char *);
api void nif_print_block(Nif *, int, char [1000]);

#pragma pack(push, 1) // important

struct NifHeader
{
	char *header_string;
	char *version;
	unsigned int unknown_1;
	unsigned char endian_type;
	unsigned int user_value;
	unsigned int num_blocks;
	unsigned int user_value_2;
	char *author;
	char *process_script;
	char *export_script;
	unsigned short num_block_types;
	char **block_types;
	unsigned short *block_type_index;
	unsigned int *block_sizes;
	unsigned int num_strings;
	unsigned int max_string_length;
	char **strings;
	unsigned int num_groups;
	unsigned int *groups;
	int end;
};

#define Vec2 struct { float x, y; }
#define Vec3 struct { float x, y, z; }
#define Vec4 struct { float x, y, z, w; }

#define Mat3 struct { float n[9]; }
#define Mat4 struct { float n[16]; }

#define Byte4 struct { unsigned char a, b, c, d; }
#define ushort3 struct { unsigned short x, y, z; }

struct ni_common_layout_t
{
	struct {
		int name;
		unsigned int num_extra_data_list;
	} * F;
	NiRef *extra_data_list;
	struct {
		NiRef controller;
		unsigned int flags;
		Vec3 translation;
		Mat3 rotation;
		float scale;
		NiRef collision_object;
	} * A;
};

NiNode
{
	struct ni_common_layout_t *common;
	struct {
		unsigned int num_children;
	} * A;
	NiRef *children;
	struct
	{
		unsigned int num_effects;
	} * B;
	NiRef *effects;
};

NiTriShape
{
	struct ni_common_layout_t *common;
	struct
	{
		NiRef data, skin_instance;
	} * A;
	struct
	{
		NiRef shader_property, alpha_property;
	} * B;
};

NiSkinInstance
{
	struct
	{
		NiRef data, skin_partition, skeleton_root;
		unsigned int num_bones;
	} * A;
	NiRef *bones;
	// skip dismemberment fields for now
	struct
	{
		int num_partitions;
	} * B;
	struct body_part_list_t *partitions;
};

legendary_edition
struct body_part_list_t
{
	unsigned short part_flag;
	unsigned short body_part;
};

NiSkinData
{
	struct
	{
		Mat3 rotation;
		Vec3 translation;
		float scale;
	} * skin_transform;
	struct
	{
		unsigned int num_bones;
		unsigned char has_vertex_weights;
	} * B;
	struct bone_data_t **bone_list;
};

legendary_edition
struct bone_data_t
{
	struct
	{
		Mat3 rotation;
		Vec3 translation;
		float scale;
	} * skin_transform;
	struct
	{
		Vec3 bounding_sphere_offset;
		float bounding_sphere_radius;
		unsigned short num_vertices;
	} * B;
	struct bone_vert_data_t *vertex_weights;
};

// todo this could be anonymous
legendary_edition
struct bone_vert_data_t
{
	unsigned short index;
	float weight;
};

legendary_edition
struct ni_skin_partition_t
{
	unsigned int *num_skin_partition_blocks;
	struct skin_partition_t **skin_partition_blocks;
};

typedef struct { float x, y, z; } Vector3;
typedef struct { unsigned short u, v; } HalfTexCoord;
typedef struct { unsigned char x, y, z; } ByteVector3;
typedef struct { unsigned char r, g, b, a; } ByteColor4;
typedef struct { unsigned short a, b, c; } ShortTriangle;

special_edition
struct ni_skin_partition_data_1
{
	Vector3 vertex;
	float bitangent_x;
	HalfTexCoord uv;
	ByteVector3 normal;
	unsigned char bitangent_y;
	ByteVector3 tangent;
	unsigned char bitangent_z;
	//ByteColor4 vertex_colors;
	struct {
		unsigned short weight1, weight2, weight3, weight4;
	} bone_weights;
	struct {
		unsigned char index1, index2, index3, index4;
	} bone_indices;
};

legendary_edition
struct skin_partition_t
{
	struct {
		unsigned int num_partitions, data_size, vertex_size;
		unsigned long long vertex_desc;
	} * A; 
};

NiTriShapeData
{
	struct
	{
		int group_id;
		unsigned short num_vertices;
		unsigned char keep_flags, compress_flags, has_vertices;
	} * A;
	Vec3 *vertices;
	struct
	{
		unsigned short bs_vector_flags;
		unsigned int material_crc;
		unsigned char has_normals;
	} * C;
	Vec3 *normals;
	Vec3 *tangents;
	Vec3 *bitangents;
	struct
	{
		Vec3 center;
		float radius;
		unsigned char has_vertex_colors;
	} * G;
	Vec4 *vertex_colors;
	Vec2 *uv_sets;
	struct
	{
		unsigned short consistency_flags;
		NiRef additional_data;
		unsigned short num_triangles;
		unsigned int num_triangle_points;
		unsigned char has_triangles;
	} * J;
	ushort3 *triangles;
	struct
	{
		unsigned short num_match_groups;
	} * L;
	NiRef *match_groups;
};

BSLightingShaderProperty
{
	struct
	{
		unsigned int skyrim_shader_type;
		int name;
		unsigned int num_extra_data_list;
	} * A;
	NiRef *extra_data_list;
	struct
	{
		NiRef controller;
		unsigned int shader_flags_1, shader_flags_2;
		Vec2 uv_offset, uv_scale;
		NiRef texture_set;
		Vec3 emissive_color;
		float emissive_multiple;
		unsigned int texture_clamp_mode;
		float alpha, refraction_strength, glossiness;
		Vec3 specular_color;
		float specular_strength, lighting_effect_1, lighting_effect_2;
	} * B;
};

BSEffectShaderProperty
{
	struct
	{
		int name;
		unsigned int num_extra_data_list;
	} * A;
	NiRef *extra_data_list;
	struct
	{
		NiRef controller;
		unsigned int shader_flags_1, shader_flags_2;
		Vec2 uv_offset, uv_scale;
	} * B;
	char *source_texture; // sized string
	struct
	{
		unsigned char texture_clamp_mode;
		unsigned char lighting_influence;
		unsigned char env_map_min_lod;
		unsigned char unknown_byte;
	} * C;
	struct {
		float start_angle, stop_angle, start_opacity, stop_opacity;
	} * falloff;
	struct {
		Vec4 base_color;
		float base_color_scale, soft_falloff_depth;
	} * D;
	char *greyscale_texture; // sized string
	struct {
		NiRef parent;
		float u, v;
	} meta;
};

BSEffectShaderPropertyFloatController
{
	struct {
		NiRef next_controller;
		unsigned short flags;
		float frequency, phase, start_time, stop_time;
		NiRef target, interpolator;
		unsigned int controlled_variable;
	} * A;
	struct {
		float time;
	} meta;
};

NiFloatInterpolator
{
	struct {
		float value;
		NiRef data;
	} * A;
};

NiFloatData
{
	struct {
		unsigned int num_keys, key_type;
	} * A;
	struct {
		float time, value;
	} *linear_keys;
	struct {
		float time, value, forward, backward;
	} *quadratic_keys;
};

BSShaderTextureSet
{
	struct
	{
		int num_textures;
	} * A;
	char **textures; // sized strings
};

NiAlphaProperty
{
	struct
	{
		int name;
		unsigned int num_extra_data_list;
	} * A;
	NiRef *extra_data_list;
	struct
	{
		NiRef controller;
		unsigned short flags;
		unsigned char treshold;
	} * C;
};

NiControllerSequence
{
	struct
	{
		int name;
		unsigned int num_controlled_blocks;
		unsigned int array_grow_by;
	} * A;
	struct controlled_block_t *controlled_blocks;
	struct
	{
		float weight;
		NiRef text_keys;
		unsigned int cycle_type;
		float frequency, start_time, stop_time;
		NiRef manager;
		int accum_root_name;
		unsigned short num_anim_note_arrays;
	} * C;
	NiRef *anim_note_arrays;
};

struct controlled_block_t
{
	NiRef interpolator;
	NiRef controller;
	unsigned char priority;
	int node_name, property_type, controller_type, controller_id, interpolator_id;
};

NiTransformInterpolator
{
	struct
	{
		Vec3 translation;
		Vec4 rotation;
		float scale;
	} * transform;
	struct
	{
		NiRef data;
	} * B;
};

NiTransformData
{
	struct
	{
		unsigned int num_rotation_keys;
	} * A;
	struct
	{
		unsigned int rotation_type;
	} * B;
	struct quaternion_key_t *quaternion_keys;
	struct
	{
		unsigned int num_keys, interpolation;
	} * translations;
	struct translation_key_t *translation_keys;
	struct
	{
		unsigned int num_keys;
	} * scales;
	NiRef *scale_keys;
};

struct quaternion_key_t
{
	float time;
	Vec4 value;
};

struct translation_key_t
{
	float time;
	Vec3 value;
};

struct ni_text_key_extra_data_t
{
	struct
	{
		int name;
		unsigned int num_text_keys;
	} * A;
	NiRef *text_keys;
};

#define offset_bs_vertex_desc(flags) \
	(unsigned short)((flags & 0xFFFFFF0000000000) >> 44)

inline void api nif_sse_dissect_vertex_desc(
	unsigned long long bs_vertex_desc,
	int *vertex, int *uvs, int *normals, int *tangents, int *colors, int *skinned)
{
	unsigned short flags = offset_bs_vertex_desc(bs_vertex_desc);
	*vertex = flags & 1 << 0x0;
	*uvs = flags & 1 << 0x1;
	*normals = flags & 1 << 0x3;
	*tangents = flags & 1 << 0x4;
	*colors = flags & 1 << 0x5;
	*skinned = flags & 1 << 0x6;
}

struct bs_vertex_data_sse_all
{
	Vector3 vertex;
	float bitangent_x;
	HalfTexCoord uv;
	ByteVector3 normal;
	unsigned char bitangent_y;
	ByteVector3 tangent;
	unsigned char bitangent_z;
	ByteColor4 vertex_colors;
};

struct bs_vertex_data_sse_no_clr
{
	Vector3 vertex;
	float bitangent_x;
	HalfTexCoord uv;
	ByteVector3 normal;
	unsigned char bitangent_y;
	ByteVector3 tangent;
	unsigned char bitangent_z;
};

BSTriShape
{
	struct ni_common_layout_t *common;
	struct {
		Vec3 center;
		float radius;
	} *bounding_sphere;
	struct {
		NiRef skin, shader_property, alpha_property;
	} *refs;
	struct {
	unsigned long long vertex_desc;
	unsigned short num_triangles, num_vertices;
	unsigned int data_size;
	} *infos;
	struct bs_vertex_data_sse_all    *vertex_data_all;
	struct bs_vertex_data_sse_no_clr *vertex_data_no_clr;
	ShortTriangle *triangles;
	unsigned int *particle_data_size;
};

#pragma pack(pop)

#undef Vec2
#undef Vec3
#undef Vec4
#undef Mat3
#undef Mat4

#endif