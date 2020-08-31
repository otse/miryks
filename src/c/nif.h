#ifndef C_NIF_H
#define C_NIF_H

#define api

typedef void ni_block_t;

typedef struct nif_t nif_t;
typedef struct rd_t rd_t;
typedef struct nif_hedr_t nif_hedr_t;
typedef struct nmap_t nmap_t;
typedef struct Vec2 Vec2;
typedef struct Vec3 Vec3;
typedef struct Vec4 Vec4;
typedef struct Mat3 Mat3;
typedef struct Mat4 Mat4;
typedef struct Ushort3 Ushort3;
typedef int32_t NiRef;
typedef int32_t ni_string_t;

typedef struct NiCommonLayout NiCommonLayout;
typedef struct NiNode NiNode;
typedef struct NiTriShape NiTriShape;
typedef struct NiTriShapeData NiTriShapeData;
typedef struct BsLightingShaderProperty BsLightingShaderProperty;
typedef struct BsShaderTextureSet BsShaderTextureSet;

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
};

//typedef void(* rd_func_t)(rd_t *, int, int, void *);

struct rd_t {
	int x;
	int *skips;
	nif_t *nif;
	void *data;
	int parent, current;
	void(* other)(rd_t *, int, int, const char *);
	void(* ni_node)(rd_t *, ni_node_t *);
	void(* ni_tri_shape)(rd_t *, ni_tri_shape_t *);
	void(* ni_tri_shape_data)(rd_t *, ni_tri_shape_data_t *);
	void(* bs_lighting_shader_property)(rd_t *, bs_lighting_shader_property_t *);
	void(* bs_shader_texture_set)(rd_t *, bs_shader_texture_set_t *);
};

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
api rd_t *nif_alloc_rd();

api void nif_free_rd(rd_t **);
api void nif_rd(nif_t *, rd_t *, void *);

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

struct Vec2{ float x, y; };
struct Vec3{ float x, y, z; };
struct Vec4{ float x, y, z, w; };
struct Mat3{ float n[9]; };
struct Mat4{ float n[16]; };
struct Ushort3{ unsigned short x, y, z; };

struct NiCommonLayout {
	int name;
	unsigned int num_extra_data_list;
	NiRef *extra_data_list, controller;
	unsigned int flags;
	Vec3 translation;
	Mat3 rotation;
	float scale;
	NiRef collision_object;
	int end;
	char *name_string;
};

// ninode, bsleafanimnode, bsfadenode
struct NiNode {
	NiCommonLayout common;
	unsigned int num_children;
	NiRef *children;
	unsigned int num_effects;
	NiRef *effects;
};

// nitrishape, bslodtrishape
struct NiTriShape {
	NiCommonLayout common;
	NiRef data, skin_instance, material_data, shader_property, alpha_property;
	int end;
};

// bstrishape, bsdynamictrishape
struct BsTriShape {
	int end;
};

// niskininstance, bsdismemberskininstance
struct NiSkinInstance {
	int end;
};

struct NiTriShapeData {
	int group_id;
	unsigned short num_vertices;
	unsigned char keep_flags, compress_flags, has_vertices;
	Vec3 *vertices;
	unsigned short bs_vector_flags;
	unsigned int material_crc;
	unsigned char has_normals;
	Vec3 *normals, *tangents, *bitangents, center;
	float radius;
	unsigned char has_vertex_colors;
	Vec4 *vertex_colors;
	Vec2 *uv_sets;
	unsigned short consistency_flags;
	NiRef additional_data;
	unsigned short num_triangles;
	unsigned int num_triangle_points;
	unsigned char has_triangles;
	Ushort3 *triangles;
	unsigned short num_match_groups;
	NiRef *match_groups;
	int end;
};

struct BsLightingShaderProperty {
	unsigned int skyrim_shader_type;
	int name;
	unsigned int num_extra_data_list;
	NiRef *extra_data_list, controller;
	unsigned int shader_flags_1, shader_flags_2;
	Vec2 uv_offset, uv_scale;
	NiRef texture_set;
	Vec3 emissive_color;
	float emissive_multiple;
	unsigned int texture_clamp_mode;
	float alpha, refraction_strength, glossiness;
	Vec3 specular_color;
	float specular_strength, lighting_effect_1, lighting_effect_2;
	int end;
	char *name_string;
};

struct BsShaderTextureSet {
	int num_textures;
	char **textures; // sized strings
	int end;
};

#pragma pack(pop)

#endif