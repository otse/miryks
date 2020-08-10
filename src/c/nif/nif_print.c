#include "c/c.h"

#include "nif.h"

#include "from_buf_helpers.h"

void print_ni_node(nifn, char *);

void nif_print_hedr(nif_t *nif, char *s)
{
	snprintf(s, 600,
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

char *print_vec_3(char *s, float *v) {
	snprintf(
		s, 100,
		"[%f, %f, %f]",
		v[0], v[1], v[2]);
	return s;
}

char *print_mat_33(char *s, float *v) {
	snprintf(
		s, 200,
		"\n   [%f, %f, %f]\n   [%f, %f, %f]\n   [%f, %f, %f]",
		v[0], v[1], v[2],
		v[3], v[4], v[5],
		v[6], v[7], v[8]);
	return s;
}

char *print_mat_44(char *s, float *v) {
	snprintf(
		s, 200,
		"\n   [%f, %f, %f, %f]\n   [%f, %f, %f, %f]\n   [%f, %f, %f, %f]\n   [%f, %f, %f, %f]",
		v[0], v[1], v[2], v[3],
		v[4], v[5], v[6], v[7],
		v[8], v[9], v[10], v[11],
		v[12], v[13], v[14], v[15]);
	return s;
}

void print_ni_node(nifn, char *s)
{
	char x[100], y[200];
	ni_node_t *block = blocks[n].v;
	snprintf(s, 600,
		"\
ninode type \
\nname: %s [%i]\
\nnum_extra_data_list: %u\
\nextra_data_list\
\ncontroller: %i\
\nflags: %u\
\ntranslation: %s\
\nrotation: %s\
\nscale: %f\
\ncollision_object: %u\
\nnum_children: %u\
\nchildren\
\nnum_effects: %u\
",
block->name_string,
block->name,
block->num_extra_data_list,
block->controller,
block->flags,
print_vec_3(x, block->translation),
print_mat_33(y, block->rotation),
block->scale,
block->collision_object,
block->num_children,
block->num_effects);
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
}