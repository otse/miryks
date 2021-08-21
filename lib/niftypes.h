#define NiNode                                "NiNode"
#define BSLeafAnimNode                        "BSLeafAnimNode"
#define BSFadeNode                            "BSFadeNode"
#define NiSkinInstance                        "NiSkinInstance"
#define BSDismemberSkinInstance               "BSDismemberSkinInstance"
#define NiSkinData                            "NiSkinData"
#define NiSkinPartition                       "NiSkinPartition"
#define BSTriShape                            "BSTriShape"
#define NiTriShape                            "NiTriShape"
#define NiTriShapeData                        "NiTriShapeData"
#define BSLODTriShape                         "BSLODTriShape"
#define BSDynamicTriShape                     "BSDynamicTriShape"
#define NiAlphaProperty                       "NiAlphaProperty"
#define BSShaderTextureSet                    "BSShaderTextureSet"
#define BSLightingShaderProperty              "BSLightingShaderProperty"
#define BSEffectShaderProperty                "BSEffectShaderProperty"
#define BSEffectShaderPropertyFloatController "BSEffectShaderPropertyFloatController"
#define NiFloatInterpolator                   "NiFloatInterpolator"
#define NiFloatData                           "NiFloatData"
#define NiControllerSequence                  "NiControllerSequence"
#define NiTextKeyExtraData                    "NiTextKeyExtraData"
#define NiStringExtraData                     "NiStringExtraData"
#define NiTransformInterpolator               "NiTransformInterpolator"
#define NiTransformData                       "NiTransformData"
#define BSDecalPlacementVectorExtraData       "BSDecalPlacementVectorExtraData"


#define nif_type(x) 0 == strcmp(block_type, x)
#define nif_types(x, y, z) (nif_type(x)) || (y ? nif_type(y) : 0) || (z ? nif_type(z) : 0)

#define ninode struct ni_node_t *
#define nicommonlayout struct ni_common_layout_t *
#define bslightingshaderproperty struct bs_lighting_shader_property_t *
#define bseffectshaderproperty struct bs_effect_shader_property_t *
#define bseffectshaderpropertyfloatcontroller struct bs_effect_shader_property_float_controller_t *
#define nifloatinterpolator struct ni_float_interpolator_t *
#define nifloatdata struct ni_float_data_t *
#define bsshadertextureset struct bs_shader_texture_set_t *
#define nialphaproperty struct ni_alpha_property_t *
#define nicontrollersequence struct ni_controller_sequence_t *
#define nitransforminterpolator struct ni_transform_interpolator_t *
#define nitransformdata struct ni_transform_data_t *
#define nitrishape struct ni_tri_shape_t *
#define nitrishapedata struct ni_tri_shape_data_t *
#define niskininstance struct ni_skin_instance_t *
#define niskindata struct ni_skin_data_t *
#define niskinpartition struct ni_skin_partition_t *
#define bstrishape struct bs_tri_shape_t *

