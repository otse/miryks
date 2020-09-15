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
	int *skips;
	struct nifp *nif;
	void *data;
	int parent, current;
	void(* other)(struct nifprd *, int, int, const char *);
	void(* ni_node)(struct nifprd *, struct ni_node *);
	void(* ni_tri_shape)(struct nifprd *, struct ni_tri_shape *);
	void(* ni_tri_shape_data)(struct nifprd *, struct ni_tri_shape_data *);
	void(* bs_lighting_shader_property)(struct nifprd *, struct bs_lighting_shader_property *);
	void(* bs_shader_texture_set)(struct nifprd *, struct bs_shader_texture_set *);
	void(* ni_alpha_property)(struct nifprd *, struct ni_alpha_property *);
};

void nifp_gui();
void nifp_test();

void nifp_read_header(struct nifp *);
void nifp_read_blocks(struct nifp *);

api struct nifp   *malloc_nifp();
api struct nifprd *malloc_nifprd();

api void free_nifrd(struct nifprd **);
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
	unsigned int user_value, num_blocks, user_value_2;
	char *author, *process_script, *export_script;
	unsigned short num_block_types;
	char **block_types;
	unsigned short *block_type_index;
	unsigned int *block_sizes, num_strings, max_string_length;
	char **strings;
	unsigned int num_groups, *groups;
	int end;
};

struct ni_common_layout_pointer {
	struct
	{
	int name;
	unsigned int num_extra_data_list;
	} *A;
	ni_ref *extra_data_list;
	struct {
	ni_ref controller;
	unsigned int flags;
	struct vec_3p translation;
	struct mat_3p rotation;
	float scale;
	ni_ref collision_object;
	} *C;
};

struct ni_node_pointer {
	struct ni_common_layout_pointer *common;
	struct {
	unsigned int num_children;
	} *A;
	ni_ref *children;
	struct {
	unsigned int num_effects;
	} *C;
	ni_ref *effects;
};

struct ni_tri_shape_pointer {
	struct ni_common_layout_pointer *common;
	struct
	{
	ni_ref data, skin_instance;
	} *A;
	struct {
	ni_ref shader_property, alpha_property;
	} *B;
};

struct bs_tri_shape_pointer_pointer {
	int end;
};

struct ni_skin_instance_pointer {
	int end;
};

struct ni_tri_shape_data_pointer {
	struct {
	int group_id;
	unsigned short num_vertices;
	unsigned char keep_flags, compress_flags, has_vertices;
	} *A;
	struct vec_3p *vertices;
	struct {
	unsigned short bs_vector_flags;
	unsigned int material_crc;
	unsigned char has_normals;
	} *C;
	struct vec_3p *normals;
	struct vec_3p *tangents;
	struct vec_3p *bitangents;
	struct {
	struct vec_3p center;
	float radius;
	unsigned char has_vertex_colors;
	} *G;
	struct vec_4p *vertex_colors;
	struct vec_2p *uv_sets;
	struct {
	unsigned short consistency_flags;
	ni_ref additional_data;
	unsigned short num_triangles;
	unsigned int num_triangle_points;
	unsigned char has_triangles;
	} *J;
	struct ushort_3p *triangles;
	struct {
	unsigned short num_match_groups;
	} *L;
	ni_ref *match_groups;
};
static int test1 = sizeof(struct ni_tri_shape_data_pointer);

struct bs_lighting_shader_property_pointer {
	struct {
	unsigned int skyrim_shader_type;
	int name;
	unsigned int num_extra_data_list;
	} *A;
	ni_ref *extra_data_list;
	struct {
	ni_ref controller;
	unsigned int shader_flags_1, shader_flags_2;
	struct vec_2p uv_offset, uv_scale;
	ni_ref texture_set;
	struct vec_3p emissive_color;
	float emissive_multiple;
	unsigned int texture_clamp_mode;
	float alpha, refraction_strength, glossiness;
	struct vec_3p specular_color;
	float specular_strength, lighting_effect_1, lighting_effect_2;
	} *B;
};

struct bs_shader_texture_set_pointer {
	struct {
	int num_textures;
	} *A;
	char **textures; // sized strings
};

struct ni_alpha_property_pointer {
	struct {
	int name;
	unsigned int num_extra_data_list;
	} *A;
	ni_ref *extra_data_list;
	struct {
	ni_ref controller;
	unsigned short flags;
	unsigned char treshold;
	} *C;
};

#pragma pack(pop)

#endif