#include "c/c.h"

#include "nif.h"

char *nif_print_hedr(nif_t *nif)
{
	char *buf = malloc(600 * sizeof(char));
	int w = snprintf(
		buf, 600,
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
	return buf;
}