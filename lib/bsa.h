#ifndef C_BSA_H
#define C_BSA_H

// part of gloom

// for oldrim

// 100% based on https://github.com/Ortham/libbsa

#define api

#define BSA_MAX_SEARCHES 60

struct Bsa;
struct Rc;

struct bsa_hedr
{
	char id[4];
	unsigned long ver, offset, archive_flags, folders, files, foldersl, filesl, file_flags;
};

struct bsa_fld
{
	unsigned long long hash;
	unsigned long num, offset;
};

struct bsa_file
{
	unsigned long long hash;
	unsigned long size, offset;
};

typedef struct Rc
{
	struct Bsa *bsa;
	int i, j, r;
	int size;
	const char *name;
	const unsigned char *buf;
	char path[255];
} Rc;

typedef struct Bsa
{
	char *path, filename[260];
	struct bsa_hedr hdr;
	void *stream;
	struct bsa_fld *fld;
	struct bsa_file **file;
	Rc **rc;
	int *r;
	const char **ca;
	const char **cb;
	int unimportant;
} Bsa;


api Bsa *bsa_load(const char *);
api void bsa_free(Bsa **);

api void bsa_print_hedr(Bsa *, char *s);
api void bsa_print_fld_rcd(Bsa *, char *s, int);
api void bsa_print_fle_rcd(Bsa *, char *s, int, int);
api void bsa_print_rc(Bsa *, char *s, int);

api int bsa_read(Rc *);
api Rc *bsa_find(Bsa *, const char *);
api Rc *bsa_find_more(const char *, unsigned long);

api void bsa_search(Bsa *, Rc *[BSA_MAX_SEARCHES], const char *, int *);

api Bsa **get_archives();

api Bsa *bsa_get(const char *);

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

#endif