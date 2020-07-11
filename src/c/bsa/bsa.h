#ifndef BSA_BSA_H
#define BSA_BSA_H

#define api

typedef struct bsa_t bsa_t;
typedef struct bsa_hedr_t bsa_hedr_t;
typedef struct bsa_fld_t bsa_fld_t;
typedef struct bsa_fle_t bsa_fle_t;

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

struct bsa_fle_t
{
	unsigned long long hash;
	unsigned long size, offset;
};

struct rc_t
{
	bsa_t *b;
	int i, j, r;
	const char *name;
	const char *path;
	const unsigned char *buf, *inf;
};

struct bsa_t
{
	bsa_hedr_t hdr;
	void *stream;
	//unsigned pos;
	bsa_fld_t *fld;
	bsa_fle_t **fle;
	rc_t **rc;
	int *r;
	const char **ca;
	const char **cb;
	int magic;
};

void bsa_test();
void bsa_gui();

api bsa_t bsa_load(const char *);
void bsa_read_folder_records(bsa_t *);
void bsa_read_file_records(bsa_t *);
void bsa_read_filenames(bsa_t *);

void bsa_resources(bsa_t *);
void bsa_bsort(bsa_t *);

char *bsa_path(bsa_t *, int, int);
char *bsa_read_bzstring(bsa_t *);

char *bsa_print_hedr(bsa_t *);
char *bsa_print_fld_rcd(bsa_t *, int);
char *bsa_print_fle_rcd(bsa_t *, int, int);
char *bsa_print_rc(bsa_t *, int);

api rc_t *bsa_find(bsa_t *, const char *);
api int bsa_read(bsa_t *, rc_t *);

#endif