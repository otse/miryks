#ifndef LIB_NIFP_H
#define LIB_NIFP_H

/// nif with pointers

#define api

#define special_edition

#define legendary_edition

typedef int ni_ref;

struct nifp_hedr;

struct Nifp;
struct NifpRd;

typedef struct Nifp
{
	int num;
	const char *path;
	const unsigned char *buf;
	unsigned pos;
	struct nifp_hedr *hdr;
	void **blocks;
} Nifp;

#define Callback(x) void (*x ## _callback) (struct NifpRd *, struct x ## _pointer *);

typedef struct NifpRd
{
	int x;
	char *skips;
	Nifp *nif;
	void *data;
	int parent, current;
	void (*other) (struct NifpRd *, void *block_pointer);
	Callback(ni_node)
	Callback(ni_tri_shape)
	Callback(ni_tri_shape_data)
	Callback(bs_lighting_shader_property)
	Callback(bs_effect_shader_property)
	Callback(bs_shader_texture_set)
	Callback(ni_alpha_property)
	Callback(ni_controller_sequence)
	Callback(ni_skin_instance)
	Callback(ni_skin_data)
	Callback(ni_skin_partition)
	special_edition
	Callback(bs_tri_shape)
} NifpRd;

#undef Callback

void nifp_test();

void nifp_read_header(Nifp *);
void nifp_read_blocks(Nifp *);

api Nifp   *calloc_nifp();
api NifpRd *calloc_nifprd();

api void free_nifp  (Nifp **);
api void free_nifprd(NifpRd **);

api void nifp_rd(NifpRd *);

api void nifp_read(Nifp *);
api void nifp_save(void *, Nifp *);
api Nifp *nifp_saved(void *);

api char *nifp_get_string(Nifp *, int);
api char *nifp_get_block_type(Nifp *, int);
api void *nifp_get_block(Nifp *, int);

api void nifp_print_hedr(Nifp *, char *);
api void nifp_print_block(Nifp *, int, char [1000]);

#define NIFP_GET_BLOCK(y, x, nif, i)  struct x *y = (struct x *)nifp_get_block(nif, i)

struct nifppair
{
	void *key;
	Nifp *value;
};
extern int nifs;
extern struct nifppair nifmap[5000];

#pragma pack(push, 1)

typedef struct { float x, y; } Vec2;
typedef struct { float x, y, z; } Vec3;
typedef struct { float x, y, z, w; } Vec4;
typedef struct { float n[9]; } Mat3;
typedef struct { float n[16]; } Mat4;
typedef struct { unsigned char a, b, c, d; } Byte4;
typedef struct { unsigned short x, y, z; } ushort3;

struct nifp_hedr
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

struct ni_common_layout_pointer
{
	struct {
		int name;
		unsigned int num_extra_data_list;
	} * F; // fuckup
	ni_ref *extra_data_list;
	struct {
		ni_ref controller;
		unsigned int flags;
		Vec3 translation;
		Mat3 rotation;
		float scale;
		ni_ref collision_object;
	} * A;
};

struct ni_node_pointer
{
	struct ni_common_layout_pointer *common;
	struct {
		unsigned int num_children;
	} * A;
	ni_ref *children;
	struct
	{
		unsigned int num_effects;
	} * B;
	ni_ref *effects;
};

legendary_edition struct ni_tri_shape_pointer
{
	struct ni_common_layout_pointer *common;
	struct
	{
		ni_ref data, skin_instance;
	} * A;
	struct
	{
		ni_ref shader_property, alpha_property;
	} * B;
};

#define offset_bs_vertex_desc(flags) \
	(unsigned short)((flags & 0xFFFFFF0000000000) >> 44)

inline void api nifp_sse_dissect_vertex_desc(
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

typedef struct { float x, y, z; } Vector3;
typedef struct { unsigned short u, v; } HalfTexCoord;
typedef struct { unsigned char x, y, z; } ByteVector3;
typedef struct { unsigned char r, g, b, a; } ByteColor4;
typedef struct { unsigned short a, b, c; } ShortTriangle;

special_edition struct bs_vertex_data_sse_all
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

special_edition struct bs_vertex_data_sse_no_clr
{
	Vector3 vertex;
	float bitangent_x;
	HalfTexCoord uv;
	ByteVector3 normal;
	unsigned char bitangent_y;
	ByteVector3 tangent;
	unsigned char bitangent_z;
};

special_edition struct bs_tri_shape_pointer
{
	struct ni_common_layout_pointer *common;
	struct {
		Vec3 center;
		float radius;
	} *bounding_sphere;
	struct {
		ni_ref skin;
		ni_ref shader_property;
		ni_ref alpha_property;
	} *refs;
	struct {
	unsigned long long vertex_desc;
	unsigned short num_triangles;
	unsigned short num_vertices;
	unsigned int data_size;
	} *infos;
	struct bs_vertex_data_sse_all    *vertex_data_all;
	struct bs_vertex_data_sse_no_clr *vertex_data_no_clr;
	ShortTriangle *triangles;
	unsigned int *particle_data_size;
};

legendary_edition struct ni_skin_instance_pointer
{
	struct
	{
		ni_ref data;
		ni_ref skin_partition;
		ni_ref skeleton_root;
		unsigned int num_bones;
	} * A;
	ni_ref *bones;
	// skip dismemberment fields for now
	struct
	{
		int num_partitions;
	} * B;
	struct body_part_list *partitions;
};

legendary_edition struct body_part_list
{
	unsigned short part_flag;
	unsigned short body_part;
};

legendary_edition struct ni_skin_data_pointer
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
	struct bone_data **bone_list;
};

legendary_edition struct bone_data
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
	struct bone_vert_data *vertex_weights;
};

legendary_edition struct bone_vert_data
{
	unsigned short index;
	float weight;
};

legendary_edition struct ni_skin_partition_pointer
{
	unsigned int *num_skin_partition_blocks;
	struct skin_partition **skin_partition_blocks;
};

legendary_edition struct skin_partition
{
	struct
	{
		unsigned short num_vertices;
		unsigned short num_triangles;
		unsigned short num_bones;
		unsigned short num_strips;
		unsigned short num_weights_per_vertex;
	} * A;
	unsigned short *bones;
	unsigned char *has_vertex_map;
	unsigned short *vertex_map;
	unsigned char *has_vertex_weights;
	Vec4 *vertex_weights;
	unsigned short *strip_lengths;
	unsigned char *has_faces;
	ushort3 *triangles;
	unsigned char *has_bone_indices;
	Byte4 *bone_indices;
	unsigned short *unknown_short;
};

legendary_edition struct ni_tri_shape_data_pointer
{
	struct
	{
		int group_id;
		unsigned short num_vertices;
		unsigned char keep_flags;
		unsigned char compress_flags;
		unsigned char has_vertices;
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
		ni_ref additional_data;
		unsigned short num_triangles;
		unsigned int num_triangle_points;
		unsigned char has_triangles;
	} * J;
	ushort3 *triangles;
	struct
	{
		unsigned short num_match_groups;
	} * L;
	ni_ref *match_groups;
};

struct bs_lighting_shader_property_pointer
{
	struct
	{
		unsigned int skyrim_shader_type;
		int name;
		unsigned int num_extra_data_list;
	} * A;
	ni_ref *extra_data_list;
	struct
	{
		ni_ref controller;
		unsigned int shader_flags_1;
		unsigned int shader_flags_2;
		Vec2 uv_offset;
		Vec2 uv_scale;
		ni_ref texture_set;
		Vec3 emissive_color;
		float emissive_multiple;
		unsigned int texture_clamp_mode;
		float alpha;
		float refraction_strength;
		float glossiness;
		Vec3 specular_color;
		float specular_strength;
		float lighting_effect_1;
		float lighting_effect_2;
	} * B;
};

struct bs_effect_shader_property_pointer {
	struct
	{
		int name;
		unsigned int num_extra_data_list;
	} * A;
	ni_ref *extra_data_list;
	struct
	{
		ni_ref controller;
		unsigned int shader_flags_1;
		unsigned int shader_flags_2;
		Vec2 uv_offset;
		Vec2 uv_scale;
	} * B;
	char *source_texture; // sized string
	struct
	{
		unsigned char texture_clamp_mode;
		unsigned char lighting_influence;
		unsigned char env_map_min_lod;
		unsigned char unknown_byte;
		float falloff_start_angle;
		float falloff_stop_angle;
		float falloff_start_opacity;
		float falloff_stop_opacity;
		Vec4 emissive_color;
		float emissive_multiple;
		float soft_falloff_depth;
	} * C;
	char *greyscale_texture; // sized string
};

struct bs_shader_texture_set_pointer
{
	struct
	{
		int num_textures;
	} * A;
	char **textures; // sized strings
};

struct ni_alpha_property_pointer
{
	struct
	{
		int name;
		unsigned int num_extra_data_list;
	} * A;
	ni_ref *extra_data_list;
	struct
	{
		ni_ref controller;
		unsigned short flags;
		unsigned char treshold;
	} * C;
};

struct ni_controller_sequence_pointer
{
	struct
	{
		int name;
		unsigned int num_controlled_blocks;
		unsigned int array_grow_by;
	} * A;
	struct controlled_block_pointer *controlled_blocks;
	struct
	{
		float weight;
		ni_ref text_keys;
		unsigned int cycle_type;
		float frequency;
		float start_time;
		float stop_time;
		ni_ref manager;
		int accum_root_name;
		unsigned short num_anim_note_arrays;
	} * C;
	ni_ref *anim_note_arrays;
};

struct controlled_block_pointer
{
	ni_ref interpolator;
	ni_ref controller;
	unsigned char priority;
	int node_name;
	int property_type;
	int controller_type;
	int controller_id;
	int interpolator_id;
};

struct ni_transform_interpolator_pointer
{
	struct
	{
		Vec3 translation;
		Vec4 rotation;
		float scale;
	} * transform;
	struct
	{
		ni_ref data;
	} * B;
};

struct ni_transform_data_pointer
{
	struct
	{
		unsigned int num_rotation_keys;
	} * A;
	struct
	{
		unsigned int rotation_type;
	} * B;
	struct quaternion_key_pointer *quaternion_keys;
	struct
	{
		unsigned int num_keys;
		unsigned int interpolation;
	} * translations;
	struct translation_key_pointer *translation_keys;
	struct
	{
		unsigned int num_keys;
	} * scales;
	ni_ref *scale_keys;
};

struct quaternion_key_pointer
{
	float time;
	Vec4 value;
};

struct translation_key_pointer
{
	float time;
	Vec3 value;
};

struct ni_text_key_extra_data_pointer
{
	struct
	{
		int name;
		unsigned int num_text_keys;
	} * A;
	ni_ref *text_keys;
};

#pragma pack(pop)

#endif