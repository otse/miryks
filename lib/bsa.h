#ifndef C_BSA_H
#define C_BSA_H

#define api

// wrote for dark2

#define BSA_MAX_SEARCHES 60

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

struct rc
{
	struct bsa *bsa;
	int i, j, r;
	int size;
	const char *name;
	const unsigned char *buf;
	char path[255];
};

struct bsa
{
	char *path;
	struct bsa_hedr hdr;
	void *stream;
	struct bsa_fld *fld;
	struct bsa_file **file;
	struct rc **rc;
	int *r;
	const char **ca;
	const char **cb;
	int loadlisted;
};

api struct bsa *bsa_load(const char *);
api void bsa_free(struct bsa **);

api void bsa_print_hedr(struct bsa *, char *s);
api void bsa_print_fld_rcd(struct bsa *, char *s, int);
api void bsa_print_fle_rcd(struct bsa *, char *s, int, int);
api void bsa_print_rc(struct bsa *, char *s, int);

api int bsa_read(struct rc *);
api struct rc *bsa_find(struct bsa *, const char *);
api struct rc *bsa_find_more(const char *, unsigned long);

api void bsa_search(struct bsa *, struct rc *[BSA_MAX_SEARCHES], const char *, int *);

api struct bsa **get_archives();

api struct bsa *bsa_get(const char *);

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