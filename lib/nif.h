#ifndef C_NIF_H
#define C_NIF_H

#define api

typedef struct nif Nif;
typedef struct rd Rd;

typedef int ni_ref;

struct nif_hedr
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

struct nif
{
	int n;
	const char *path;
	const unsigned char *buf;
	unsigned pos;
	struct nif_hedr hdr;
	void **blocks;
};

//typedef void(* rd_func_t)(struct rd *, int, int, void *);

struct rd {
	int x;
	int *skips;
	struct nif *nif;
	void *data;
	int parent, current;
	void(* other)(struct rd *, int, int, const char *);
	void(* ni_node)(struct rd *, struct ni_node *);
	void(* ni_tri_shape)(struct rd *, struct ni_tri_shape *);
	void(* ni_tri_shape_data)(struct rd *, struct ni_tri_shape_data *);
	void(* bs_lighting_shader_property)(struct rd *, struct bs_lighting_shader_property *);
	void(* bs_shader_texture_set)(struct rd *, struct bs_shader_texture_set *);
};

struct nmap_t {
	void *key;
	struct nif *value;
};
extern struct nmap_t nmap[1000];
extern int nifs;

void nif_gui();
void nif_test();

char *nif_read_short_string(struct nif *);
char *nif_read_sized_string(struct nif *);

void nif_read_header(struct nif *);
void nif_read_blocks(struct nif *);

api struct nif *nif_alloc();
api struct rd *nif_alloc_rd();

api void nif_free_rd(struct rd **);
api void nif_rd(struct nif *, struct rd *, void *);

api void nif_read(struct nif *);
api void nif_save(void *, struct nif *);
api struct nif *nif_saved(void *);

api char *nif_get_string(struct nif *, int);
api char *nif_get_block_type(struct nif *, int);
api void *nif_get_block(struct nif *, int);

api void nif_print_hedr(struct nif *, char *);
api void nif_print_block(struct nif *, int, char *);

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

struct vec_2{ float x, y; };
struct vec_3{ float x, y, z; };
struct vec_4{ float x, y, z, w; };
struct mat_3{ float n[9]; };
struct mat_4{ float n[16]; };
struct ushort_3{ unsigned short x, y, z; };

struct ni_common_layout {
	int name;
	unsigned int num_extra_data_list;
	ni_ref *extra_data_list, controller;
	unsigned int flags;
	struct vec_3 translation;
	struct mat_3 rotation;
	float scale;
	ni_ref collision_object;
	int end;
	char *name_string;
};

// ninode, bsleafanimnode, bsfadenode
struct ni_node {
	struct ni_common_layout common;
	unsigned int num_children;
	ni_ref *children;
	unsigned int num_effects;
	ni_ref *effects;
};

// nitrishape, bslodtrishape
struct ni_tri_shape {
	struct ni_common_layout common;
	ni_ref data, skin_instance, material_data, shader_property, alpha_property;
	int end;
};

// bstrishape, bsdynamictrishape
struct bs_tri_shape {
	int end;
};

// niskininstance, bsdismemberskininstance
struct ni_skin_instance {
	int end;
};

struct ni_tri_shape_data {
	int group_id;
	unsigned short num_vertices;
	unsigned char keep_flags, compress_flags, has_vertices;
	struct vec_3 *vertices;
	unsigned short bs_vector_flags;
	unsigned int material_crc;
	unsigned char has_normals;
	struct vec_3 *normals, *tangents, *bitangents, center;
	float radius;
	unsigned char has_vertex_colors;
	struct vec_4 *vertex_colors;
	struct vec_2 *uv_sets;
	unsigned short consistency_flags;
	ni_ref additional_data;
	unsigned short num_triangles;
	unsigned int num_triangle_points;
	unsigned char has_triangles;
	struct ushort_3 *triangles;
	unsigned short num_match_groups;
	ni_ref *match_groups;
	int end;
};

struct bs_lighting_shader_property {
	unsigned int skyrim_shader_type;
	int name;
	unsigned int num_extra_data_list;
	ni_ref *extra_data_list, controller;
	unsigned int shader_flags_1, shader_flags_2;
	struct vec_2 uv_offset, uv_scale;
	ni_ref texture_set;
	struct vec_3 emissive_color;
	float emissive_multiple;
	unsigned int texture_clamp_mode;
	float alpha, refraction_strength, glossiness;
	struct vec_3 specular_color;
	float specular_strength, lighting_effect_1, lighting_effect_2;
	int end;
	char *name_string;
};

struct bs_shader_texture_set {
	int num_textures;
	char **textures; // sized strings
	int end;
};

#pragma pack(pop)

#endif