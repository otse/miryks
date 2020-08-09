#ifndef NIF_NIF_H
#define NIF_NIF_H

#include "ni_list.h"

// https://gist.github.com/EmilHernvall/953968

#define api

typedef struct nif_t nif_t;
typedef struct nif_hedr_t nif_hedr_t;
typedef struct nif_block_t nif_block_t;
typedef struct nif_ni_node_t nif_ni_node_t;
typedef struct nmap_t nmap_t;

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
	// short strings
	char *author, *process_script, *export_script;
	unsigned short num_block_types;
	// sized strings
	char **block_types;
	unsigned short *block_type_index;
	unsigned int *block_sizes;
	unsigned int num_strings;
	unsigned int max_string_length;
	// sized strings
	char **strings;
	unsigned int num_groups;
	// sized strings
	char **groups;
	int end;
};

struct nif_block_t {
	int n;
};

struct nif_t
{
	int pos;
	nif_hedr_t hdr;
	nif_block_t **blocks;
	int n;
	const char *path;
	const unsigned char *buf;
};

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

void nif_read_header(nif_t *);
void nif_read_header_string(nif_t *);
void nif_read_some_stuff(nif_t *);
void nif_read_block_types(nif_t *);
void nif_read_block_type_index(nif_t *);
void nif_read_block_sizes(nif_t *);
void nif_read_strings(nif_t *);
void nif_read_groups(nif_t *);

void nif_read_blocks(nif_t *);
void nif_read_block(nif_t *, int);

api nif_t *nif_alloc();

api void nif_make(void *, nif_t *);
api nif_t *nif_get_stored(void *);

void nif_add(void *, nif_t *);

api char *nif_print_hedr(nif_t *);
api char *nif_print_block(nif_t *, int);

// block types

struct nif_ni_node_t {
	nif_block_t base;
	
};


#endif