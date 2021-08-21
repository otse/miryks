#include "common.h"

#include "nif.h"
#include "niftypes.h"

#define Hedr   nif->hdr
#define Blocks nif->blocks

api void nif_print_hedr(Nif *nif, char *s)
{
	snprintf(
		s, 600,
		"\
header string: %s\
\nversion: %s\
\nendian type: %u\
\nuser value: %u\
\nnum blocks: %u\
\nuser value 2: %u\
\nauthor: %s\
\nprocess script: %s\
\nexport script: %s\
\nnum block types: %hu\
\nblock types\
\nblock type index\
\nblock sizes\
\nnum strings: %u\
\nmax string length: %u\
\nstrings\
\nnum groups: %u\
\ngroups\
",
		Hedr->header_string,
		Hedr->version,
		Hedr->endian_type,
		Hedr->user_value,
		Hedr->num_blocks,
		Hedr->user_value_2,
		Hedr->author,
		Hedr->process_script,
		Hedr->export_script,
		Hedr->num_block_types,
		Hedr->num_strings,
		Hedr->max_string_length,
		Hedr->num_groups);
}

static char *print_vec_2p(char *s, float *f)
{
	snprintf(s, 200, "[%f, %f]", f[0], f[1]);
	return s;
}

static char *print_vec_3p(char *s, float *f)
{
	snprintf(s, 200, "[%f, %f, %f]", f[0], f[1], f[2]);
	return s;
}

static char *print_vec_4p(char *s, float *f)
{
	snprintf(s, 200, "[%f, %f, %f, %f]", f[0], f[1], f[2], f[3]);
	return s;
}

static char *print_ushort_3p(char *s, unsigned short *h)
{
	snprintf(s, 200, "[%hu, %hu, %hu]", h[0], h[1], h[2]);
	return s;
}

static char *print_mat_3p(char *s, float *f)
{
#define V ((float *)&v)
	snprintf(
		s, 200,
		"\
\n   [%f, %f, %f] \
\n   [%f, %f, %f] \
\n   [%f, %f, %f]",
		f[0], f[1], f[2],
		f[3], f[4], f[5],
		f[6], f[7], f[8]);
	return s;
}

static char *print_mat_4p(char *s, float *v)
{
#define V ((float *)&v)
	snprintf(
		s, 200,
		"\
\n   [%f, %f, %f, %f] \
\n   [%f, %f, %f, %f] \
\n   [%f, %f, %f, %f] \
\n   [%f, %f, %f, %f]",
		V[0], V[1], V[2], V[3],
		V[4], V[5], V[6], V[7],
		V[8], V[9], V[10], V[11],
		V[12], V[13], V[14], V[15]);
	return s;
}

static char *print_ni_common_layout(Nif *nif, char s[600], struct ni_common_layout_t *block)
{
	char x[200], y[200];
	snprintf(
		s, 600,
		"\
name: %s [%i]\
\nnum_extra_data_list: %u\
\nextra_data_list\
\ncontroller: %i\
\nflags: %u\
\ntranslation: %s\
\nrotation: %s\
\nscale: %f\
\ncollision_object: %i\
",
		nif_get_string(nif, block->F->name),
		block->F->name,
		block->F->num_extra_data_list,
		block->A->controller,
		block->A->flags,
		print_vec_3p(x, (float *)&block->A->translation),
		print_mat_3p(y, (float *)&block->A->rotation),
		block->A->scale,
		block->A->collision_object);
	return s;
}

static void print_ni_node(Nif *nif, int n, char s[1000])
{
	char x[600];
	struct ni_node_t *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
%s\
\nni node: \
\nnum_children: %u\
\nchildren\
\nnum_effects: %u\
",
		print_ni_common_layout(nif, x, block->common),
		block->A->num_children,
		block->B->num_effects);
}

#ifdef SLE

static void print_ni_tri_shape(Nif *nif, int n, char s[1000])
{
	char x[600];
	struct ni_tri_shape_t *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
%s\
\nni tri shape: \
\ndata: %i\
\nskin_instance: %i\
\nmaterial data\
\nshader_property: %i\
\nalpha_property: %i\
",
		print_ni_common_layout(nif, x, block->common),
		block->A->data,
		block->A->skin_instance,
		block->B->shader_property,
		block->B->alpha_property);
}

static void print_ni_tri_shape_data(Nif *nif, int n, char s[1000])
{
	char a[200], b[200], c[200], d[200], e[200], f[200], g[200], h[200], i[200], j[200], k[200], l[200], m[200], o[200], p[200];
	struct ni_tri_shape_data_t *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
group_id: %i\
\nnum_vertices: %hu\
\nkeep_flags: %u\
\ncompress_flags: %u\
\nhas_vertices: %i\
\nvertices\
\nvertices 1: %s \
\nvertices %i: %s \
\nbs_vector_flags dec: %hu\
\nmaterial crc: %i\
\nhas_normals: %i\
\nnormals\
\nnormals 1: %s \
\nnormals %i: %s \
\ntangents\
\ntangents 1: %s \
\ntangents %i: %s \
\nbitangents\
\nbitangents 1: %s\
\nbitangents %i: %s\
\ncenter: %s\
\nradius: %f\
\nhas_vertex_colors: %i\
\nvertex_colors\
\nvertex_colors 1: %s\
\nvertex_colors: %i: %s\
\nuv_sets\
\nuv_sets 1: %s\
\nuv_sets %i: %s\
\nconsistency_flags: %hu\
\nadditional_data: %u\
\nnum_triangles: %hu\
\nnum_triangle_points: %hu\
\nhas_triangles: %i\
\ntriangles\
\ntriangles 1: %s\
\ntriangles %i: %s\
\nnum_match_groups: %hu\
\nmatch_groups\
",
		block->A->group_id,
		block->A->num_vertices,
		block->A->keep_flags,
		block->A->compress_flags,
		block->A->has_vertices,
		print_vec_3p(a, (float *)&block->vertices[0]),
		block->A->num_vertices,
		print_vec_3p(b, (float *)&block->vertices[block->A->num_vertices - 1]),
		block->C->bs_vector_flags,
		block->C->material_crc,
		block->C->has_normals,
		block->C->has_normals ? print_vec_3p(c, (float *)&block->normals[0]) : "",
		block->A->num_vertices,
		block->C->has_normals ? print_vec_3p(d, (float *)&block->normals[block->A->num_vertices - 1]) : "",
		print_vec_3p(e, (float *)&block->tangents[0]),
		block->A->num_vertices,
		print_vec_3p(f, block->tangents[block->A->num_vertices - 1]),
		print_vec_3p(g, block->bitangents[0]),
		block->A->num_vertices,
		print_vec_3p(h, block->bitangents[block->A->num_vertices - 1]),
		print_vec_3p(i, block->G->center),
		block->G->radius,
		block->G->has_vertex_colors,
		block->G->has_vertex_colors ? print_vec_4p(j, block->vertex_colors[0]) : "",
		block->A->num_vertices,
		block->G->has_vertex_colors ? print_vec_4p(k, block->vertex_colors[block->A->num_vertices - 1]) : "",
		print_vec_2p(l, block->uv_sets[0]),
		block->A->num_vertices,
		print_vec_2p(m, block->uv_sets[block->A->num_vertices - 1]),
		block->J->consistency_flags,
		block->J->additional_data,
		block->J->num_triangles,
		block->J->num_triangle_points,
		block->J->has_triangles,
		print_ushort_3p(o, block->triangles[0]),
		block->J->num_triangles,
		block->J->has_triangles ? print_ushort_3p(p, block->triangles[block->J->num_triangles - 1]) : "",
		block->L->num_match_groups);
}

#endif

static void print_bs_lighting_shader_property(Nif *nif, int n, char s[1000])
{
	char a[200], b[200], c[200], d[200];
	BSLightingShaderProperty *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
skyrim_shader_type: %u\
\nname: %s [%i]\
\nnum_extra_data_list: %u\
\nextra_data_list: -\
\ncontroller: %i\
\nshader_flags_1: %u\
\nshader_flags_2: %u\
\nuv_offset: %s\
\nuv_scale: %s\
\ntexture_set: %i\
\nemissive_color: %s\
\nemissive_multiple: %f\
\ntexture_clamp_mode: %u\
\nalpha: %f\
\nrefraction_strength: %f\
\nglossiness: %f\
\nspecular_color: %s\
\nspecular_strength: %f\
\nlighting_effect_1: %f\
\nlighting_effect_2: %f\
",
		block->A->skyrim_shader_type,
		nif_get_string(nif, block->A->name),
		block->A->name,
		block->A->num_extra_data_list,
		block->B->controller,
		block->B->shader_flags_1,
		block->B->shader_flags_2,
		print_vec_2p(a, (float *)&block->B->uv_offset),
		print_vec_2p(b, (float *)&block->B->uv_scale),
		block->B->texture_set,
		print_vec_3p(c, (float *)&block->B->emissive_color),
		block->B->emissive_multiple,
		block->B->texture_clamp_mode,
		block->B->alpha,
		block->B->refraction_strength,
		block->B->glossiness,
		print_vec_3p(d, (float *)&block->B->specular_color),
		block->B->specular_strength,
		block->B->lighting_effect_1,
		block->B->lighting_effect_2);
}

static void print_bs_shader_texture_set(Nif *nif, int n, char s[1000])
{
	BSShaderTextureSet *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
num_textures: %i\
\ntextures 0: %s\
\ntextures 1: %s\
\ntextures 2: %s\
\ntextures 3: %s\
\ntextures 4: %s\
\ntextures 5: %s\
\ntextures 6: %s\
\ntextures 7: %s\
\ntextures 8: %s\
",
		block->A->num_textures,
		block->textures[0],
		block->textures[1],
		block->textures[2],
		block->textures[3],
		block->textures[4],
		block->textures[5],
		block->textures[6],
		block->textures[7],
		block->textures[8]);
}

static void print_bs_effect_shader_property(Nif *nif, int n, char s[1000])
{
	char a[200], b[200], c[200], d[200];
	BSEffectShaderProperty *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
effect shader property\
\nname: %i [%s]\
\nnum_extra_data_list: %i\
\ncontroller: %i\
\nshader_flags_1: %u\
\nshader_flags_2: %u\
\nuv_offset: %f %f\
\nuv_scale: %f %f\
\nsource_texture: %s\
\ntexture_clamp_mode: %u\
\nlighting_influence: %u\
\nenv_map_min_lod: %u\
\nunknown_byte: %u\
\nfallof_start_angle: %f\
\nfallof_stop_angle: %f\
\nfallof_start_opacity: %f\
\nfallof_stop_opacity: %f\
\nebase_color: [%f, %f, %f, %f]\
\nebase_color_scale: %f\
\nsoft_faloff_depth: %f\
\ngreyscale_texture: %s\
",
block->A->name,
nif_get_string(nif, block->A->name),
block->A->num_extra_data_list,
block->B->controller,
block->B->shader_flags_1,
block->B->shader_flags_2,
block->B->uv_offset.x,
block->B->uv_offset.y,
block->B->uv_scale.x,
block->B->uv_scale.y,
block->source_texture,
block->C->texture_clamp_mode,
block->C->lighting_influence,
block->C->env_map_min_lod,
block->C->unknown_byte,
block->falloff->start_angle,
block->falloff->stop_angle,
block->falloff->start_opacity,
block->falloff->stop_opacity,
block->D->base_color.x,
block->D->base_color.y,
block->D->base_color.z,
block->D->base_color.w,
block->D->base_color_scale,
block->D->soft_falloff_depth,
block->greyscale_texture
);
}

static void print_ni_controller_sequence(Nif *nif, int n, char s[1000])
{
	NiControllerSequence *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
name: %i\
\nnum_controlled_blocks: %u\
\narray_grow_by: %u\
\ncontrolled blocks: -\
\nweight: %f\
\ntext_Keys: %i\
\ncycle_type: %u\
\nfrequency: %f\
\nstart_time: %f\
\nstop_time: %f\
\nmanager: %i\
\naccum_root_name: %i\
\nnum_anim_note_arrays: %hu\
",
		block->A->name,
		block->A->num_controlled_blocks,
		block->A->array_grow_by,
		block->C->weight,
		block->C->text_keys,
		block->C->cycle_type,
		block->C->frequency,
		block->C->start_time,
		block->C->stop_time,
		block->C->manager,
		block->C->accum_root_name,
		block->C->num_anim_note_arrays);
}

static void print_ni_transform_interpolator(Nif *nif, int n, char s[1000])
{
	char a[200], b[200];//, c[200], d[200];
	struct ni_transform_interpolator_t *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
transform:\
\n translation: %s\
\n rotation: %s\
\n scale: %f\
\ndata: %i\
",
		print_vec_3p(a, (float *)&block->transform->translation),
		print_vec_4p(b, (float *)&block->transform->rotation),
		block->transform->scale,
		block->B->data);
}

static void print_ni_transform_data(Nif *nif, int n, char s[1000])
{
	NiTransformData *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
num_rotation_keys: %u\
\nrotation_type: %u\
\nquaternion_keys: -\
\ntranslations:\
\n num_keys: %u\
\n interpolation: %u\
\n keys: -\
\nscales:\
\n num_keys: %u\
\n keys: -\
",
		block->A->num_rotation_keys,
		block->A->num_rotation_keys > 0 ? block->B->rotation_type : 0,
		block->translations->num_keys,
		block->translations->interpolation,
		block->scales->num_keys);
}

static void print_ni_skin_instance(Nif *nif, int n, char s[1000])
{
	NiSkinInstance *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
data: %i\
\nskin_partition: %i\
\nskeleton_root: %i\
\nnum_bones: %u\
\nbones: -\
\nnum_partitions: %i\
\npartitions: -\
",
		block->A->data,
		block->A->skin_partition,
		block->A->skeleton_root,
		block->A->num_bones,
		block->B->num_partitions);
}

static void print_ni_skin_data(Nif *nif, int n, char s[1000])
{
	char a[200], b[200];//, c[200], d[200];
	NiSkinData *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
skin_transform:\
\n rotation: %s\
\n translation: %s\
\n scale: %f\
\nnum_bones: %u\
\nhas_vertex_weights: %u\
\nbone_list: -\
",
		print_mat_3p(a, (float *)&block->skin_transform->rotation),
		print_vec_3p(b, (float *)&block->skin_transform->translation),
		block->skin_transform->scale,
		block->B->num_bones,
		block->B->has_vertex_weights);
}

static char *print_skin_partition(char *, struct skin_partition_t *);

static void print_ni_skin_partition(Nif *nif, int n, char s[1000])
{
	char a[1000];
	NiSkinPartition *block = Blocks[n];
	snprintf(
		s, 1000,
		"\
num_skin_partition_blocks: %u\
\nskin_partition_blocks: -\
\nskin_partition 0:\
\n%s\
",
		*block->num_skin_partition_blocks,
		(block->num_skin_partition_blocks > 0 ? print_skin_partition(a, block->skin_partition_blocks[0]) : NULL));
}

static char *print_skin_partition(char *s, struct skin_partition_t *skin_partition)
{
	/*
	snprintf(
		s, 1000,
		"\
 num_vertices: %hu\
\n num_triangles: %hu\
\n num_bones: %hu\
\n num_strips: %hu\
\n num_weights_per_vertex: %hu\
\n has_vertex_map: %u\
\n has_vertex_weights: %u\
\n has_faces: %u\
\n has_bone_indices: %u\
",
		skin_partition->A->num_vertices,
		skin_partition->A->num_triangles,
		skin_partition->A->num_bones,
		skin_partition->A->num_strips,
		skin_partition->A->num_weights_per_vertex,
		*skin_partition->has_vertex_map,
		*skin_partition->has_vertex_weights,
		*skin_partition->has_faces,
		*skin_partition->has_bone_indices
		);
	return s;*/
	return s;
}

static void print_ni_alpha_property(Nif *nif, int n, char s[1000])
{
	const char *blendModes[] = {
		"One",
		"Zero",
		"Src Color",
		"Inv Src Color",
		"Dst Color",
		"Inv Dst Color",
		"Src Alpha",
		"Inv Src Alpha",
		"Dst Alpha",
		"Inv Dst Alpha",
		"Src Alpha Saturate"
	};
	const char *testModes[] = {
		"Always",
		"Less",
		"Equal",
		"Less or Equal",
		"Greater",
		"Not Equal",
		"Greater or Equal",
		"Never"
	};
	char x[600];
	struct ni_alpha_property_t *block = Blocks[n];
	unsigned short flags = block->C->flags;
	snprintf(
		s, 1000,
		"\
name: %s [%i]\
\nnum_extra_data_list: %u\
\nextra_data_list\
\ncontroller: %i\
\nflags: %u\
\nthreshold: %u\
\n ---\
\nblending enabled: %s\
\nsource blend mode: %s\
\ndestination blend mode: %s\
\ntesting enabled: %s\
\nalpha test function: %s\
\nalpha test threshold: %u\
",
		nif_get_string(nif, block->A->name),
		block->A->name,
		block->A->num_extra_data_list,
		block->C->controller,
		block->C->flags,
		block->C->treshold,
		flags & 1 ? "yes" : "no",
		blendModes[flags >> 1 & 0x0f],
		blendModes[flags >> 5 & 0x0f],
		flags & ( 1 << 9 ) ? "yes" : "no",
		testModes[flags >> 10 & 0x07],
		block->C->treshold
		//testModes[1]
	);
}

api void nif_print_block(Nif *nif, int n, char s[1000])
{
	s[0] = '\0';
	const char *block_type = Hedr->block_types[Hedr->block_type_index[n]];
	if (0);
	else if ( nif_types(NiNodeS, BSLeafAnimNodeS, BSFadeNodeS) ) print_ni_node(nif, n, s);
	#ifdef SLE
	else if ( nif_type(NiTriShapeS) ) print_ni_tri_shape(nif, n, s);
	else if ( nif_type(NiTriShapeData) ) print_ni_tri_shape_data(nif, n, s);
	#endif
	else if ( nif_type(BSLightingShaderPropertyS) ) print_bs_lighting_shader_property(nif, n, s);
	else if ( nif_type(BSEffectShaderPropertyS) ) print_bs_effect_shader_property(nif, n, s);
	else if ( nif_type(BSShaderTextureSetS) ) print_bs_shader_texture_set(nif, n, s);
	else if ( nif_type(NiControllerSequenceS) ) print_ni_controller_sequence(nif, n, s);
	else if ( nif_type(NiTransformInterpolatorS) ) print_ni_transform_interpolator(nif, n, s);
	else if ( nif_type(NiTransformDataS) ) print_ni_transform_data(nif, n, s);
	else if ( nif_types(NiSkinInstanceS, BSDismemberSkinInstanceS, NULL) ) print_ni_skin_instance(nif, n, s);
	else if ( nif_type(NiSkinDataS) ) print_ni_skin_data(nif, n, s);
	else if ( nif_type(NiSkinPartitionS) ) print_ni_skin_partition(nif, n, s);
	else if ( nif_type(NiAlphaPropertyS) ) print_ni_alpha_property(nif, n, s);
}