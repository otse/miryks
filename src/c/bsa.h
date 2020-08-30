#ifndef C_BSA_H
#define C_BSA_H

#define api

typedef struct Bsa Bsa;
typedef struct Bsas Bsas;
typedef struct BsaHedr BsaHedr;
typedef struct bsa_fld_t bsa_fld_t;
typedef struct bsa_file_t bsa_file_t;
typedef struct Rc Rc;

struct BsaHedr
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

struct Rc
{
	Bsa *b;
	int i, j, r;
	int size;
	const char *name;
	const unsigned char *buf;
	char path[255];
};

struct Bsa
{
	char *path;
	BsaHedr hdr;
	void *stream;
	bsa_fld_t *fld;
	bsa_file_t **file;
	Rc **rc;
	int *r;
	const char **ca;
	const char **cb;
	Bsas *bsas;
};

struct Bsas {
	int num;
	Bsa* array[30];
};

api Bsa *bsa_load(const char *);
api void bsa_free(Bsa **);

api void bsa_print_hedr(Bsa *, char *s);
api void bsa_print_fld_rcd(Bsa *, char *s, int);
api void bsa_print_fle_rcd(Bsa *, char *s, int, int);
api void bsa_print_rc(Bsa *, char *s, int);

api int bsa_read(Rc *);
api Rc *bsa_find(Bsa *, const char *);
api Rc *bsas_find(Bsas *, const char *, unsigned long);

api void bsa_search(Bsa *, Rc *[10], const char *, int *);

extern Bsas bsas;

api void bsas_add_to_loaded(Bsas *, Bsa **, int);
api Bsa *bsas_get_by_path(Bsas *, const char *);

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