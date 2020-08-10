#include "c/c.h"

#include "nif.h"

#include "from_buf_helpers.h"

char *print_ni_node(nifn);

char *nif_print_hedr(nif_t *nif)
{
	char *s = malloc(600 * sizeof(char));
	int w = snprintf(s, 600,
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
	return s;
}

char *print_vec_3(char *s, float *v) {
	int w = snprintf(
		s, 100,
		"[%f, %f, %f]",
		v[0], v[1], v[2]);
	return s;
}

char *print_mat_33(char *s, float *v) {
	int w = snprintf(
		s, 200,
		"\n   [%f, %f, %f]\n   [%f, %f, %f]\n   [%f, %f, %f]",
		v[0], v[1], v[2],
		v[3], v[4], v[5],
		v[6], v[7], v[8]);
	return s;
}

char *print_ni_node(nifn)
{
	char x[100], y[200];
	ni_node_t *ni_node = (ni_node_t *)blocks[n].v;
	char *s = malloc(1000 * sizeof(char));
	int w = snprintf(s, 1000,
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
ni_node->name_string,
ni_node->name,
ni_node->num_extra_data_list,
ni_node->controller,
ni_node->flags,
print_vec_3(x, ni_node->translation),
print_mat_33(y, ni_node->rotation),
ni_node->scale,
ni_node->collision_object,
ni_node->num_children,
ni_node->num_effects);
	return s;
}

#define type(x) 0 == strcmp(block_type, x)

char *nif_print_block(nifn)
{
	char *s = NULL;
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	if (0);
	else if (type(NI_NODE)) s = print_ni_node(nif, n);
	else if (type(BS_LEAF_ANIM_NODE)) s = print_ni_node(nif, n);
	else if (type(BS_FADE_NODE)) s = print_ni_node(nif, n);
	return s;
}