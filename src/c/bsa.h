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
	const unsigned char *buf;
	char path[255];
};

struct bsa_t
{
	char *path;
	bsa_hedr_t hdr;
	void *stream;
	bsa_fld_t *fld;
	bsa_file_t **file;
	rc_t **rc;
	int *r;
	const char **ca;
	const char **cb;
	bsas_t *bsas;
};

struct bsas_t {
	int num;
	bsa_t* array[30];
};

api bsa_t *bsa_load(const char *);
api void bsa_free(bsa_t **);

api char *bsa_print_hedr(bsa_t *);
api char *bsa_print_fld_rcd(bsa_t *, int);
api char *bsa_print_fle_rcd(bsa_t *, int, int);
api char *bsa_print_rc(bsa_t *, int);

api int bsa_read(rc_t *);
api rc_t *bsa_find(bsa_t *, const char *);
api rc_t *bsas_find(bsas_t *, const char *, unsigned long);

extern bsas_t bsas;

api void bsas_add_to_loaded(bsas_t *, bsa_t **, int);
api bsa_t *bsas_get_by_path(bsas_t *, const char *);

#define BSA_MESHES   0x1
#define BSA_TEXTURES 0x2
#define BSA_MENUS    0x4
#define BSA_SOUNDS   0x8
#define BSA_VOICES   0x10
#define BSA_SHADERS  0x20
#define BSA_TREES    0x40
#define BSA_FONTS    0x80
#define BSA_MISC     0x100

void bsa_test();
void bsa_gui();

#endif