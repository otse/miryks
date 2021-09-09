#ifndef LIB_BSA_H
#define LIB_BSA_H

// based on Ortham/libbsa

#define BSA_VER 105

#define api

#define BSA_MAX_SEARCHES 60

struct Bsa;
struct Rsc;

struct bsa_hedr
{
	char id[4];
	unsigned long ver, offset, archive_flags, folders, files, foldersl, filesl, file_flags;
};

struct bsa_fld
{
	unsigned long long hash;
#if BSA_VER==105
	unsigned long num, padding1, offset, padding2;
#else
	unsigned long num, offset;
#endif
};

struct bsa_file
{
	unsigned long long hash;
	unsigned long size, offset;
};

typedef struct Rsc
{
	struct Bsa *bsa;
	int i, j, r;
	long size;
	const char *name;
	char *buf;
	char path[260];
} Rsc;

typedef struct Bsa
{
	char filename[260];
	void *stream;
	struct bsa_hedr hdr;
	struct bsa_fld *fld;
	struct bsa_file **file;
	Rsc **rsc;
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

api int bsa_read(Rsc *);
api Rsc *bsa_find(Bsa *, const char *);
api Rsc *bsa_find_more(const char *, unsigned long);

api void bsa_search(Bsa *, Rsc *[BSA_MAX_SEARCHES], const char *, int *);

api Bsa **get_archives();

api Bsa *bsa_get(const char *);

#define BSA_MESHES 0x1
#define BSA_TEXTURES 0x2
#define BSA_MENUS 0x4
#define BSA_SOUNDS 0x8
#define BSA_VOICES 0x10
#define BSA_SHADERS 0x20
#define BSA_TREES 0x40
#define BSA_FONTS 0x80
#define BSA_MISC 0x100

void bsa_test();

#endif