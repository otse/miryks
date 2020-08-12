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

typedef struct ni_basic_layout_t ni_basic_layout_t;
typedef struct ni_node_t ni_node_t;
typedef struct ni_tri_shape_t ni_tri_shape_t;
typedef struct ni_tri_shape_data_t ni_tri_shape_data_t;
typedef struct bs_lighting_shader_property_t bs_lighting_shader_property_t;

struct ni_basic_layout_t {
	int name;
	unsigned int num_extra_data_list;
	ni_ref_t *extra_data_list;
	ni_ref_t controller;
	unsigned int flags;
	vec_3 translation;
	mat_3 rotation;
	float scale;
	ni_ref_t collision_object;
	int end;
	char *name_string;
};

struct ni_node_t {
	ni_basic_layout_t basic;
	unsigned int num_children;
	ni_ref_t *children;
	unsigned int num_effects;
	ni_ref_t *effects;
};

struct ni_tri_shape_t {
	ni_basic_layout_t basic;
	ni_ref_t data;
	ni_ref_t skin_instance;
	// skip mateiral for now
	ni_ref_t shader_property;
	ni_ref_t alpha_property;
};

struct ni_tri_shape_data_t {
	int group_id;
	unsigned short num_vertices;
	unsigned char keep_flags;
	unsigned char compress_flags;
	int has_vertices;
	vec_3 *vertices;
	int bs_vector_flags;
	unsigned int material_crc;
	int has_normals;
	vec_3 *normals;
	vec_3 *tangents;
	vec_3 *bitangents;
	vec_3 center;
	float radius;
	int has_vertex_colors;
	vec_4 *vertex_colors;
	vec_2 *uv_sets;
	unsigned short consistency_flags;
	ni_ref_t additional_data;
	unsigned short num_triangles;
	unsigned short num_triangle_points;
	int has_triangles;
	ushort_3 *triangles;
	unsigned short num_match_groups;
	int match_groups;
	int match_group;
};

struct bs_lighting_shader_property_t {
	unsigned int skyrim_shader_type;
	int name;
	unsigned int num_extra_data_list;
	ni_ref_t *extra_data_list;
	ni_ref_t controller;
	unsigned int shader_flags_1;
	unsigned int shader_flags_2;
	vec_2 uv_offset;
	vec_2 uv_scale;
	ni_ref_t texture_set;
	vec_3 emissive_color;
	float emissive_multiple;
	unsigned int texture_clamp_mode;
	float alpha;
	float refraction_strength;
	float glossiness;
	vec_3 specular_color;
	float specular_strength;
	float lighting_effect_1;
	float lighting_effect_2;
	int end;
	char *name_string;
};