#ifndef NIFP_H
#define NIFP_H

#define api

// wrote for dark2

typedef int ni_ref;

struct nifp_hedr;

struct nifp
{
	int key;
	const char *path;
	const unsigned char *buf;
	unsigned pos;
	struct nifp_hedr *hdr;
	void **blocks;
};

struct nifprd
{
	int x;
	char *skips;
	struct nifp *nif;
	void *data;
	int parent, current;
	void (*other)(struct nifprd *, int, int, const char *);
	void (*ni_node)(struct nifprd *, struct ni_node_pointer *);
	void (*ni_tri_shape)(struct nifprd *, struct ni_tri_shape_pointer *);
	void (*ni_tri_shape_data)(struct nifprd *, struct ni_tri_shape_data_pointer *);
	void (*bs_lighting_shader_property)(struct nifprd *, struct bs_lighting_shader_property_pointer *);
	void (*bs_shader_texture_set)(struct nifprd *, struct bs_shader_texture_set_pointer *);
	void (*ni_alpha_property)(struct nifprd *, struct ni_alpha_property_pointer *);
	void (*ni_controller_sequence)(struct nifprd *, struct ni_controller_sequence_pointer *);
};

void nifp_gui();
void nifp_test();

void nifp_read_header(struct nifp *);
void nifp_read_blocks(struct nifp *);

api struct nifp   *malloc_nifp();
api struct nifprd *malloc_nifprd();

api void free_nifp(struct nifp **);
api void free_nifprd(struct nifprd **);
api void nifp_rd(struct nifp *, struct nifprd *, void *);

api void nifp_read(struct nifp *);
api void nifp_save(void *, struct nifp *);
api struct nifp *nifp_saved(void *);

api char *nifp_get_string(struct nifp *, int);
api char *nifp_get_block_type(struct nifp *, int);
api void *nifp_get_block(struct nifp *, int);

api void nifp_print_hedr(struct nifp *, char *);
api void nifp_print_block(struct nifp *, int, char [1000]);

struct nifppair
{
	void *key;
	struct nifp *value;
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

struct bs_tri_shape_pointer_pointer
{
	int end;
};

struct ni_skin_instance_pointer
{
	int end;
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