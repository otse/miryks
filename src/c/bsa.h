#ifndef BSA_BSA_H
#define BSA_BSA_H

#define api

typedef struct bsa_t bsa_t;
typedef struct bsas_t bsas_t;
typedef struct bsa_hedr_t bsa_hedr_t;
typedef struct bsa_fld_t bsa_fld_t;
typedef struct bsa_file_t bsa_file_t;

typedef struct rc_t rc_t;

struct bsa_hedr_t
{
	char id[4];
	unsigned long ver, offset, archive_flags, folders, files, foldersl, filesl, file_flags;
};

struct bsa_fld_t
{
	unsigned long long hash;
	unsigned long num, offset;
};

struct bsa_file_t
{
	unsigned long long hash;
	unsigned long size, offset;
};

struct rc_t
{
	bsa_t *b;
	int i, j, r;
	int size;
	const char *name;
	char path[255];
	const unsigned char *buf;
};

struct bsa_t
{
	int num;
	char *path;
	bsa_hedr_t hdr;
	void *stream;
	bsa_fld_t *fld;
	bsa_file_t **file;
	rc_t **rc;
	int *r;
	const char **ca;
	const char **cb;
};

struct bsas_t {
	int num;
	bsa_t* bsas[30];
};

extern bsas_t bsas;

void bsa_test();
void bsa_gui();

api bsa_t *bsa_load(const char *);
api void bsa_free(bsa_t *);

char *bsa_path(bsa_t *, int, int);
char *bsa_read_bzstring(bsa_t *);
char *bsa_uncompress(char *, size_t);

api char *bsa_print_hedr(bsa_t *);
api char *bsa_print_fld_rcd(bsa_t *, int);
api char *bsa_print_fle_rcd(bsa_t *, int, int);
api char *bsa_print_rc(bsa_t *, int);

api rc_t *bsa_find(bsa_t *, const char *);
api int bsa_read_rc(rc_t *);

api rc_t *bsas_find(bsas_t *, const char *);
api void bsas_add_to_loaded(bsas_t *, bsa_t **, int);
api bsa_t *bsas_get_by_path(bsas_t *, const char *);

#endif