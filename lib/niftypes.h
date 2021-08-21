#define NiNodeS                                "NiNode"
#define BSLeafAnimNodeS                        "BSLeafAnimNode"
#define BSFadeNodeS                            "BSFadeNode"
#define NiSkinInstanceS                        "NiSkinInstance"
#define BSDismemberSkinInstanceS               "BSDismemberSkinInstance"
#define NiSkinDataS                            "NiSkinData"
#define NiSkinPartitionS                       "NiSkinPartition"
#define BSTriShapeS                            "BSTriShape"
#define NiTriShapeS                            "NiTriShape"
#define NiTriShapeDataS                        "NiTriShapeData"
#define BSLODTriShapeS                         "BSLODTriShape"
#define BSDynamicTriShapeS                     "BSDynamicTriShape"
#define NiAlphaPropertyS                       "NiAlphaProperty"
#define BSShaderTextureSetS                    "BSShaderTextureSet"
#define BSLightingShaderPropertyS              "BSLightingShaderProperty"
#define BSEffectShaderPropertyS                "BSEffectShaderProperty"
#define BSEffectShaderPropertyFloatControllerS "BSEffectShaderPropertyFloatController"
#define NiFloatInterpolatorS                   "NiFloatInterpolator"
#define NiFloatDataS                           "NiFloatData"
#define NiControllerSequenceS                  "NiControllerSequence"
#define NiTextKeyExtraDataS                    "NiTextKeyExtraData"
#define NiStringExtraDataS                     "NiStringExtraData"
#define NiTransformInterpolatorS               "NiTransformInterpolator"
#define NiTransformDataS                       "NiTransformData"
#define BSDecalPlacementVectorExtraDataS       "BSDecalPlacementVectorExtraData"


#define nif_type(x) 0 == strcmp(block_type, x)
#define nif_types(x, y, z) (nif_type(x)) || (y ? nif_type(y) : 0) || (z ? nif_type(z) : 0)

#define NiNode struct ni_node_t
#define BSLightingShaderProperty struct bs_lighting_shader_property_t
#define BSEffectShaderProperty struct bs_effect_shader_property_t
#define BSEffectShaderPropertyFloatController struct bs_effect_shader_property_float_controller_t
#define NiFloatInterpolator struct ni_float_interpolator_t
#define NiFloatData struct ni_float_data_t
#define BSShaderTextureSet struct bs_shader_texture_set_t
#define NiAlphaProperty struct ni_alpha_property_t
#define NiControllerSequence struct ni_controller_sequence_t
#define NiTransformInterpolator struct ni_transform_interpolator_t
#define NiTransformData struct ni_transform_data_t
#define NiTriShape struct ni_tri_shape_t
#define NiTriShapeData struct ni_tri_shape_data_t
#define NiSkinInstance struct ni_skin_instance_t
#define NiSkinData struct ni_skin_data_t
#define NiSkinPartition struct ni_skin_partition_t
#define BSTriShape struct bs_tri_shape_t