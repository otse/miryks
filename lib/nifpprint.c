#include "putc.h"

#include "nifp.h"

#define Hedr   nif->hdr
#define Blocks nif->blocks

void nifp_print_hedr(struct nifp *nif, char *s)
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
Hedr.header_string,
Hedr.version,
Hedr.endian_type,
Hedr.user_value,
Hedr.num_blocks,
Hedr.user_value_2,
Hedr.author,
Hedr.process_script,
Hedr.export_script,
Hedr.num_block_types,
Hedr.num_strings,
Hedr.max_string_length,
Hedr.num_groups
);
}

static char *print_vec_2p(char *s, struct vec_2p v) {
	snprintf(s, 200, "[%f, %f]", v.x, v.y);
	return s;
}

static char *print_vec_3p(char *s, struct vec_3p v) {
	snprintf(s, 200, "[%f, %f, %f]", v.x, v.y, v.z);
	return s;
}

static char *print_vec_4p(char *s, struct vec_4p v) {
	snprintf(s, 200, "[%f, %f, %f, %f]", v.x, v.y, v.z, v.w);
	return s;
}

static char *print_ushort_3p(char *s, struct ushort_3p v) {
	snprintf(s, 200, "[%hu, %hu, %hu]", v.x, v.y, v.z);
	return s;
}

static char *print_mat_3p(char *s, struct mat_3p v) {
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

static char *print_mat_4p(char *s, float *v) {
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
