#include "c/c.h"

#include "nif.h"

#include "from_buf_helpers.h"

char *print_ni_basic_layout(nifn, char *, ni_basic_layout_t *);
void print_ni_node(nifn, char *);
void print_ni_tri_shape(nifn, char *);
void print_ni_tri_shape_data(nifn, char *);
void print_bs_lighting_shader_property(nifn, char *);

void nif_print_hedr(nif_t *nif, char *s)
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
nif->hdr.header_string,
nif->hdr.version,
nif->hdr.endian_type,
nif->hdr.user_value,
nif->hdr.num_blocks,
nif->hdr.user_value_2,
nif->hdr.author,
nif->hdr.process_script,
nif->hdr.export_script,
nif->hdr.num_block_types,
nif->hdr.num_strings,
nif->hdr.max_string_length,
nif->hdr.num_groups
);
}

char *print_vec_2(char *s, vec_2 v) {
	snprintf(s, 200, "[%f, %f]", v.x, v.y);
	return s;
}

char *print_vec_3(char *s, vec_3 v) {
	snprintf(s, 200, "[%f, %f, %f]", v.x, v.y, v.z);
	return s;
}

char *print_vec_4(char *s, vec_4 v) {
	snprintf(s, 200, "[%f, %f, %f, %f]", v.x, v.y, v.z, v.w);
	return s;
}

char *print_ushort_3(char *s, ushort_3 v) {
	snprintf(s, 200, "[%hu, %hu, %hu]", v.x, v.y, v.z);
	return s;
}

char *print_mat_3(char *s, mat_3 v) {
#define V ((float*)&v)
	snprintf(
		s, 200,
		"\
\n   [%f, %f, %f] \
\n   [%f, %f, %f] \
\n   [%f, %f, %f]",
V[0], V[1], V[2],
V[3], V[4], V[5],
V[6], V[7], V[8]);
	return s;
}

char *print_mat_4(char *s, float *v) {
#define V ((float*)&v)
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


char *print_ni_basic_layout(nifn, char *s, ni_basic_layout_t *block)
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
block->name_string,
block->name,
block->num_extra_data_list,
block->controller,
block->flags,
print_vec_3(x, block->translation),
print_mat_3(y, block->rotation),
block->scale,
block->collision_object);
	return s;
}

void print_ni_node(nifn, char *s)
{
	char x[600];
	ni_node_t *block = blocks[n].v;
	snprintf(
		s, 1000,
		"\
%s\
\nni node: \
\nnum_children: %u\
\nchildren\
\nnum_effects: %u\
",
print_ni_basic_layout(nif, n, x, &block->basic),
block->num_children,
block->num_effects);
}

void print_ni_tri_shape(nifn, char *s)
{
	char x[600];
	ni_tri_shape_t *block = blocks[n].v;
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
print_ni_basic_layout(nif, n, x, &block->basic),
block->data,
block->skin_instance,
block->shader_property,
block->alpha_property);
}

void print_ni_tri_shape_data(nifn, char *s)
{
	char a[200], b[200], c[200], d[200], e[200], f[200], g[200], h[200], i[200], j[200], k[200], l[200], m[200], o[200], p[200];
	ni_tri_shape_data_t *block = blocks[n].v;
	snprintf(
		s, 1500,
		"\
group_id: %i\
\nnum_vertices: %hu\
\nkeep_flags: %u\
\ncompress_flags: %u\
\nhas_vertices: %i\
\nvertices\
\nvertices 1: %s\
",
block->group_id,
block->num_vertices,
block->keep_flags,
block->compress_flags,
block->has_vertices,
print_vec_3(a, block->vertices[0])
);
}

void print_bs_lighting_shader_property(nifn, char *s)
{
	char a[200], b[200], c[200], d[200];
	bs_lighting_shader_property_t *block = blocks[n].v;
	snprintf(
		s, 1500,
		"\
skyrim_shader_type: %u\
\nname: %s [%i]\
\nnum_extra_data_list: %u\
\nextra_data_list\
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
block->skyrim_shader_type,
block->name_string,
block->name,
block->num_extra_data_list,
block->controller,
block->shader_flags_1,
block->shader_flags_2,
print_vec_2(a, block->uv_offset),
print_vec_2(b, block->uv_scale),
block->texture_set,
print_vec_3(c, block->emissive_color),
block->emissive_multiple,
block->texture_clamp_mode,
block->alpha,
block->refraction_strength,
block->glossiness,
print_vec_3(d, block->specular_color),
block->specular_strength,
block->lighting_effect_1,
block->lighting_effect_2
);
}

#define type(x) 0 == strcmp(block_type, x)

void nif_print_block(nifn, char *s)
{
	s[0] = '\0';
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	if (0);
	else if (type(NI_NODE)) print_ni_node(nif, n, s);
	else if (type(BS_LEAF_ANIM_NODE)) print_ni_node(nif, n, s);
	else if (type(BS_FADE_NODE)) print_ni_node(nif, n, s);
	else if (type(NI_TRI_SHAPE)) print_ni_tri_shape(nif, n, s);
	else if (type(NI_TRI_SHAPE_DATA)) print_ni_tri_shape_data(nif, n, s);
	else if (type(BS_LIGHTING_SHADER_PROPERTY)) print_bs_lighting_shader_property(nif, n, s);
}