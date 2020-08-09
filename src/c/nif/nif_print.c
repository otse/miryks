#include "c/c.h"

#include "nif.h"

#include "from_buf_helpers.h"

char *print_ni_node(nifn);

char *nif_print_hedr(nif_t *nif)
{
	char *s = malloc(600 * sizeof(char));
	int w = snprintf(
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
	return s;
}

char *print_ni_node(nifn)
{
	ni_node_t *ni_node = (ni_node_t *)nif->blocks[n];
	char *s = malloc(600 * sizeof(char));
	int w = snprintf(
		s, 600,
		"\
ninode type \
\nstring: %i\
\nversion: %i\
",
2,
2
);
	return s;
}

#define type(x) 0 == strcmp(block_type, x)

char *nif_print_block(nifn)
{
	char *s = NULL;
	const char *block_type = hedr.block_types[hedr.block_type_index[n]];
	if (0) ;
	else if (type(NI_NODE)) s = print_ni_node(nif, n);
	else if (type(BS_LEAF_ANIM_NODE)) s = print_ni_node(nif, n);
	else if (type(BS_FADE_NODE)) s = print_ni_node(nif, n);
	return s;
}