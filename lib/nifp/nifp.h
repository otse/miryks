#ifndef NIFP_H
#define NIFP_H

#define api

// wrote for gloom/dark
// for oldrim

typedef int ni_ref;

struct nifp_hedr;

struct Nifp;
struct NifpRd;

typedef struct Nifp
{
	int key;
	const char *path;
	const unsigned char *buf;
	unsigned pos;
	struct nifp_hedr *hdr;
	void **blocks;
} Nifp;

typedef struct NifpRd
{
	int x;
	char *skips;
	Nifp *nif;
	void *data;
	int parent, current;
	void (*other)						(struct NifpRd *, void *block_pointer);
	void (*ni_node)						(struct NifpRd *, struct ni_node_pointer *);
	void (*ni_tri_shape)				(struct NifpRd *, struct ni_tri_shape_pointer *);
	void (*ni_tri_shape_data)			(struct NifpRd *, struct ni_tri_shape_data_pointer *);
	void (*bs_lighting_shader_property)	(struct NifpRd *, struct bs_lighting_shader_property_pointer *);
	void (*bs_effect_shader_property)	(struct NifpRd *, struct bs_effect_shader_property_pointer *);
	void (*bs_shader_texture_set)		(struct NifpRd *, struct bs_shader_texture_set_pointer *);
	void (*ni_alpha_property)			(struct NifpRd *, struct ni_alpha_property_pointer *);
	void (*ni_controller_sequence)		(struct NifpRd *, struct ni_controller_sequence_pointer *);
	void (*ni_skin_instance)			(struct NifpRd *, struct ni_skin_instance_pointer *);
	void (*ni_skin_data)				(struct NifpRd *, struct ni_skin_data_pointer *);
	void (*ni_skin_partition)			(struct NifpRd *, struct ni_skin_partition_pointer *);
} NifpRd;

void nifp_test();

void nifp_read_header(Nifp *);
void nifp_read_blocks(Nifp *);

api Nifp   *malloc_nifp();
api NifpRd *malloc_nifprd();

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

struct nifppair
{
	void *key;
	Nifp *value;
};
extern int nifps;
extern struct nifppair nifpmap[1000];

#pragma pack(push, 1)

struct vec_2p{ float x, y; };
struct vec_3p{ float x, y, z; };
struct vec_4p{ float x, y, z, w; };
struct mat_3p{ float n[9]; };
struct mat_4p{ float n[16]; };
struct ushort_3p{ unsigned short x, y, z; };
struct byte_4p{ unsigned char a, b, c, d; };

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
	struct
	{
		int name;
		unsigned int num_extra_data_list;
	} * A;
	ni_ref *extra_data_list;
	struct
	{
		ni_ref controller;
		unsigned int flags;
		struct vec_3p translation;
		struct mat_3p rotation;
		float scale;
		ni_ref collision_object;
	} * C;
};

struct ni_node_pointer
{
	struct ni_common_layout_pointer *common;
	struct
	{
		unsigned int num_children;
	} * A;
	ni_ref *children;
	struct
	{
		unsigned int num_effects;
	} * C;
	ni_ref *effects;
};

struct ni_tri_shape_pointer
{
	struct ni_common_layout_pointer *common;
	struct
	{
		ni_ref data;
		ni_ref skin_instance;
	} * A;
	struct
	{
		ni_ref shader_property;
		ni_ref alpha_property;
	} * B;
};

struct bs_tri_shape_pointer
{
	int end;
};

struct ni_skin_instance_pointer
{
	struct
	{
		ni_ref data;
		ni_ref skin_partition;
		ni_ref skeleton_root;
		unsigned int num_bones;
	} * A;
	ni_ref *bones;
	// dismemberment fields
	struct
	{
		int num_partitions;
	} * B;
	struct body_part_list *partitions;
};

struct body_part_list
{
	unsigned short part_flag;
	unsigned short body_part;
};

struct ni_skin_data_pointer
{
	struct
	{
		struct mat_3p rotation;
		struct vec_3p translation;
		float scale;
	} * skin_transform;
	struct
	{
		unsigned int num_bones;
		unsigned char has_vertex_weights;
	} * B;
	struct bone_data **bone_list;
};

struct bone_data
{
	struct
	{
		struct mat_3p rotation;
		struct vec_3p translation;
		float scale;
	} * skin_transform;
	struct
	{
		struct vec_3p bounding_sphere_offset;
		float bounding_sphere_radius;
		unsigned short num_vertices;
	} * B;
	struct bone_vert_data *vertex_weights;
};

struct bone_vert_data
{
	unsigned short index;
	float weight;
};

struct ni_skin_partition_pointer
{
	unsigned int *num_skin_partition_blocks;
	struct skin_partition **skin_partition_blocks;
};

struct skin_partition
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
	struct vec_4p *vertex_weights;
	unsigned short *strip_lengths;
	unsigned char *has_faces;
	struct ushort_3p *triangles;
	unsigned char *has_bone_indices;
	struct byte_4p *bone_indices;
	unsigned short *unknown_short;
};

struct ni_tri_shape_data_pointer
{
	struct
	{
		int group_id;
		unsigned short num_vertices;
		unsigned char keep_flags;
		unsigned char compress_flags;
		unsigned char has_vertices;
	} * A;
	struct vec_3p *vertices;
	struct
	{
		unsigned short bs_vector_flags;
		unsigned int material_crc;
		unsigned char has_normals;
	} * C;
	struct vec_3p *normals;
	struct vec_3p *tangents;
	struct vec_3p *bitangents;
	struct
	{
		struct vec_3p center;
		float radius;
		unsigned char has_vertex_colors;
	} * G;
	struct vec_4p *vertex_colors;
	struct vec_2p *uv_sets;
	struct
	{
		unsigned short consistency_flags;
		ni_ref additional_data;
		unsigned short num_triangles;
		unsigned int num_triangle_points;
		unsigned char has_triangles;
	} * J;
	struct ushort_3p *triangles;
	struct
	{
		unsigned short num_match_groups;
	} * L;
	ni_ref *match_groups;
};
static int test1 = sizeof(struct ni_tri_shape_data_pointer);

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
		struct vec_2p uv_offset;
		struct vec_2p uv_scale;
		ni_ref texture_set;
		struct vec_3p emissive_color;
		float emissive_multiple;
		unsigned int texture_clamp_mode;
		float alpha;
		float refraction_strength;
		float glossiness;
		struct vec_3p specular_color;
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
		struct vec_2p uv_offset;
		struct vec_2p uv_scale;
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
		struct vec_4p emissive_color;
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
		struct vec_3p translation;
		struct vec_4p rotation;
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
	struct vec_4p value;
};

struct translation_key_pointer
{
	float time;
	struct vec_3p value;
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