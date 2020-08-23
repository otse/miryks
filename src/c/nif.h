#ifndef NIF_NIF_H
#define NIF_NIF_H

#define api

typedef void ni_block_t;

typedef struct nif_t nif_t;
typedef struct rd_t rd_t;
typedef struct nif_hedr_t nif_hedr_t;
typedef struct nmap_t nmap_t;
typedef struct vec_2 vec_2;
typedef struct vec_3 vec_3;
typedef struct vec_4 vec_4;
typedef struct mat_3 mat_3;
typedef struct mat_4 mat_4;
typedef struct ushort_3 ushort_3;
typedef int32_t ni_ref_t;
typedef int32_t ni_string_t;

typedef struct ni_common_layout_t ni_common_layout_t;
typedef struct ni_node_t ni_node_t;
typedef struct ni_tri_shape_t ni_tri_shape_t;
typedef struct ni_tri_shape_data_t ni_tri_shape_data_t;
typedef struct bs_lighting_shader_property_t bs_lighting_shader_property_t;
typedef struct bs_shader_texture_set_t bs_shader_texture_set_t;

struct nif_hedr_t
{
	char *header_string;
	unsigned int unknown_1;
	char *version;
	unsigned char endian_type;
	unsigned int user_value, num_blocks, user_value_2;
	char *author, *process_script, *export_script; // short strings
	unsigned short num_block_types;
	char **block_types; // sized strings
	unsigned short *block_type_index;
	unsigned int *block_sizes, num_strings, max_string_length;
	char **strings; // sized strings
	unsigned int num_groups, *groups;
	int end;
};

struct nif_t
{
	int n;
	const char *path;
	const unsigned char *buf;
	unsigned pos;
	nif_hedr_t hdr;
	ni_block_t **blocks;
	int *skips;
};

typedef void(* rd_func_t)(rd_t *, int, int, void *);

struct rd_t {
	int x;
	nif_t *nif;
	void *data;
	int parent, current;
	void(* other)(rd_t *, int, int, const char *);
	//rd_func_t typed;
	void(* ni_node)(rd_t *, ni_node_t *, int, int);
	void(* ni_tri_shape)(rd_t *, ni_tri_shape_t *, int, int);
};

struct vec_2{ float x, y; };
struct vec_3{ float x, y, z; };
struct vec_4{ float x, y, z, w; };
struct mat_3{ float n[9]; };
struct mat_4{ float n[16]; };
struct ushort_3{ unsigned short x, y, z; };

struct nmap_t {
	void *key;
	nif_t *value;
};
extern nmap_t nmap[1000];
extern int nifs;

void nif_gui();
void nif_test(void *);

char *nif_read_short_string(nif_t *);
char *nif_read_sized_string(nif_t *);

void nif_read_header(nif_t *);
void nif_read_blocks(nif_t *);

api nif_t *nif_alloc();

api rd_t *nif_alloc_rundown();
api void nif_accept(nif_t *, rd_t *, void *);

api void nif_read(nif_t *);
api void nif_save(void *, nif_t *);
api nif_t *nif_saved(void *);

api char *nif_get_string(nif_t *, int);
api char *nif_get_block_type(nif_t *, int);
api ni_block_t *nif_get_block(nif_t *, int);

api void nif_print_hedr(nif_t *, char *);
api void nif_print_block(nif_t *, int, char *);

///

#define NI_NODE "NiNode"
#define BS_LEAF_ANIM_NODE "BSLeafAnimNode"
#define BS_FADE_NODE "BSFadeNode"
#define NI_SKIN_INSTANCE "NiSkinInstance"
#define BS_DISMEMBER_SKIN_INSTANCE "BSDismemberSkinInstance"
#define NI_SKIN_DATA "NiSkinData"
#define NI_SKIN_PARTITION "NiSkinPartition"
#define BS_TRI_SHAPE "BSTriShape"
#define BS_DYNAMIC_TRI_SHAPE "BSDynamicTriShape"
#define NI_TRI_SHAPE "NiTriShape"
#define BS_LOD_TRI_SHAPE "BSLODTriShape"
#define NI_ALPHA_PROPERTY "NiAlphaProperty"
#define NI_TRI_SHAPE_DATA "NiTriShapeData"
#define BS_EFFECT_SHADER_PROPERTY "BSEffectShaderProperty"
#define BS_EFFECT_SHADER_PROPERTY_FLOAT_CONTROLLER "BSEffectShaderPropertyFloatController"
#define NI_FLOAT_INTERPOLATOR "NiFloatInterpolator"
#define NI_FLOAT_DATA "NiFloatData"
#define BS_LIGHTING_SHADER_PROPERTY "BSLightingShaderProperty"
#define BS_SHADER_TEXTURE_SET "BSShaderTextureSet"
#define NI_CONTROLLER_SEQUENCE "NiControllerSequence"
#define NI_TEXT_KEY_EXTRA_DATA "NiTextKeyExtraData"
#define NI_STRING_EXTRA_DATA "NiStringExtraData"
#define NI_TRANSFORM_INTERPOLATOR "NiTransformInterpolator"
#define NI_TRANSFORM_DATA "NiTransformData"
#define BS_DECAL_PLACEMENT_VECTOR_EXTRA_DATA "BSDecalPlacementVectorExtraData"

#define ni_is_type(x) 0 == strcmp(block_type, x)
#define ni_is_any(x, y, z) ni_is_type(x) || (y ? ni_is_type(y) : 0) || (z ? ni_is_type(z) : 0)

#pragma pack(push, 1)

struct ni_common_layout_t {
	int name;
	unsigned int num_extra_data_list;
	ni_ref_t *extra_data_list, controller;
	unsigned int flags;
	vec_3 translation;
	mat_3 rotation;
	float scale;
	ni_ref_t collision_object;
	int end;
	char *name_string;
};

// ninode, bsleafanimnode, bsfadenode
struct ni_node_t {
	ni_common_layout_t common;
	unsigned int num_children;
	ni_ref_t *children;
	unsigned int num_effects;
	ni_ref_t *effects;
};

// nitrishape, bslodtrishape
struct ni_tri_shape_t {
	ni_common_layout_t common;
	ni_ref_t data, skin_instance, material_data, shader_property, alpha_property;
	int end;
};

// bstrishape, bsdynamictrishape
struct bs_tri_shape {
	int end;
};

// niskininstance, bsdismemberskininstance
struct ni_skin_instance_t {
	int end;
};

struct ni_tri_shape_data_t {
	int group_id;
	unsigned short num_vertices;
	unsigned char keep_flags, compress_flags, has_vertices;
	vec_3 *vertices;
	unsigned short bs_vector_flags;
	unsigned int material_crc;
	unsigned char has_normals;
	vec_3 *normals, *tangents, *bitangents, center;
	float radius;
	unsigned char has_vertex_colors;
	vec_4 *vertex_colors;
	vec_2 *uv_sets;
	unsigned short consistency_flags;
	ni_ref_t additional_data;
	unsigned short num_triangles;
	unsigned int num_triangle_points;
	unsigned char has_triangles;
	ushort_3 *triangles;
	unsigned short num_match_groups;
	int *match_groups;
	int end;
};

struct bs_lighting_shader_property_t {
	unsigned int skyrim_shader_type;
	int name;
	unsigned int num_extra_data_list;
	ni_ref_t *extra_data_list, controller;
	unsigned int shader_flags_1, shader_flags_2;
	vec_2 uv_offset, uv_scale;
	ni_ref_t texture_set;
	vec_3 emissive_color;
	float emissive_multiple;
	unsigned int texture_clamp_mode;
	float alpha, refraction_strength, glossiness;
	vec_3 specular_color;
	float specular_strength, lighting_effect_1, lighting_effect_2;
	int end;
	char *name_string;
};

struct bs_shader_texture_set_t {
	int num_textures;
	char **textures; // sized strings
	int end;
};

#pragma pack(pop)

#endif