#ifndef NIF_NIF_H
#define NIF_NIF_H

#define api

typedef struct ni_block_t ni_block_t;

typedef struct nif_t nif_t;
typedef struct nif_hedr_t nif_hedr_t;
typedef struct nmap_t nmap_t;
typedef struct vec_2 vec_2;
typedef struct vec_3 vec_3;
typedef struct vec_4 vec_4;
typedef struct mat_3 mat_3;
typedef struct mat_4 mat_4;
typedef struct ushort_3 ushort_3;
typedef int32_t ni_ref_t;
typedef int32_t ni_string_t;

#define t_short_string char *
#define t_sized_string char *

struct nif_hedr_t
{
	char *header_string;
	unsigned int unknown_1;
	char *version;
	unsigned char endian_type;
	unsigned int user_value;
	unsigned int num_blocks;
	unsigned int user_value_2;
	char *author, *process_script, *export_script; // short strings
	unsigned short num_block_types;
	char **block_types;
	unsigned short *block_type_index; // sized strings
	unsigned int *block_sizes;
	unsigned int num_strings;
	unsigned int max_string_length;
	char **strings; // sized strings
	unsigned int num_groups;
	unsigned int *groups; // sized strings
	unsigned end;
};

struct ni_block_t
{
	int n;
	void *v;
};

struct nif_t
{
	int n;
	const char *path;
	const unsigned char *buf;
	unsigned pos;
	nif_hedr_t hdr;
	ni_block_t *blocks;
};

struct vec_2{ float x, y; };
struct vec_3{ float x, y, z; };
struct vec_4{ float x, y, z, w; };
struct mat_3{ float n[9]; };
struct mat_4{ float n[16]; };
struct ushort_3{ unsigned short x, y, z; };

#include "ni_blocks.h"

struct nmap_t {
	void *key;
	nif_t *value;
};
extern nmap_t nmap[1000];
extern int nifs;

void nif_gui();
void nif_test(void *);

char *nif_read_short_string(nif_t *);
char *nif_read_sized_string(nif_t *);

api nif_t *nif_alloc();
api void nif_make(void *, nif_t *);
api void nif_add(void *, nif_t *);
api nif_t *nif_get_stored(void *);

api void nif_print_hedr(nif_t *, char *);
api void nif_print_block(nif_t *, int, char *);

api void nif_read_header(nif_t *);
api void nif_read_blocks(nif_t *);

#endif