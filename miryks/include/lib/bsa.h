#ifndef LIB_BSA_H
#define LIB_BSA_H

// based on Ortham/libbsa

#define BSA_VER 105

#define api

#define BSA_MAX_SEARCHES 60

#define BSA Bsa * 
#define RES resource *

struct Bsa;
struct resource;

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

typedef struct resource
{
	struct BSA bsa;
	int i, j, r;
	long size;
	const char *name;
	char *buf;
	char path[260];
} resource;

typedef struct Bsa
{
	char filename[260];
	void *stream;
	struct bsa_hedr hdr;
	struct bsa_fld *fld;
	struct bsa_file **file;
	RES*res;
	int *r;
	const char **ca;
	const char **cb;
	int unimportant;
} Bsa;

api BSA bsa_load(const char *);
api void bsa_free(BSA *);

api void bsa_print_hedr(BSA, char *s);
api void bsa_print_fld_rcd(BSA, char *s, int);
api void bsa_print_fle_rcd(BSA, char *s, int, int);
api void bsa_print_rc(BSA, char *s, int);

api int bsa_read(RES);
api RES bsa_find(BSA, const char *);
api RES bsa_find_more(const char *, unsigned long);

api void bsa_search(BSA, RES[BSA_MAX_SEARCHES], const char *, int *);

api BSA *get_archives();

api BSA bsa_get(const char *);

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