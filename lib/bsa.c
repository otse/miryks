#include "putc.h"

#include "bsa.h"

// based on
// https://github.com/Ortham/libbsa/blob/master/src/api/tes4bsa.cpp

#include <zlib.h>

#define BSA "bsa - "
#define VER 104
#define VER_SE 105

#define Hedr bsa->hdr

static int read(struct bsa *, void *, unsigned);
static int seek(struct bsa *, unsigned);

char *bsa_read_bzstring(struct bsa *);

void read_folder_records(struct bsa *);
void read_file_records(struct bsa *);
void read_filenames(struct bsa *);
void resources(struct bsa *);

api struct bsa *bsa_load(const char *path)
{
	struct bsa *bsa = malloc(sizeof(struct bsa));
	memset(bsa, 0, sizeof(struct bsa));
	bsa->path = malloc(sizeof(char) * strlen(path) + 1);
	strcpy(bsa->path, path);
	bsa->stream = fopen(path, "rb");
	cassert(
		bsa->stream, path);
	read(bsa, &Hedr, sizeof(struct bsa_hedr));
	cassert(
		strcmp(
			"BSA\x00", (char *)&Hedr.id) == 0,
		BSA "not a bsa");
	cassert(
		Hedr.ver != VER_SE, BSA "cant use special edition");
	cassert(
		Hedr.ver == VER, BSA "not 104");
	read_folder_records(bsa);
	read_file_records(bsa);
	read_filenames(bsa);
	resources(bsa);
	//printf("loaded bsa %s\n", path);
	return bsa;
}

void read_folder_records(struct bsa *bsa)
{
	bsa->fld = malloc(sizeof(struct bsa_fld) * Hedr.folders);
	read(bsa, bsa->fld, Hedr.folders * sizeof(struct bsa_fld));
}

char *bsa_read_bzstring(struct bsa *bsa)
{
	int c = 0;
	read(bsa, &c, 1);
	char *name = malloc(sizeof(char) * c);
	read(bsa, name, c);
	return name;
}

void read_file_records(struct bsa *bsa)
{
	bsa->file = malloc(sizeof(struct bsa_file *) * Hedr.folders);
	bsa->ca = malloc(sizeof(char *) * Hedr.folders);
	for (int i = 0; i < Hedr.folders; i++)
	{
	const int num = bsa->fld[i].num;
	bsa->file[i] = malloc(sizeof(struct bsa_file) * num);
	bsa->ca[i] = bsa_read_bzstring(bsa);
	read(bsa, bsa->file[i], sizeof(struct bsa_file) * num);
	}
}

void read_filenames(struct bsa *bsa)
{
	char *buf = malloc(sizeof(char) * Hedr.filesl);
	bsa->cb = malloc(sizeof(char *) * Hedr.files);
	read(bsa, buf, Hedr.filesl);
	int i = 0, j = 0, n = 0;
	while (i++ < Hedr.filesl)
	{
	if (buf[i] != '\0')
	continue;
	bsa->cb[n++] = &buf[j];
	j = i + 1;
	if (n >= Hedr.files)
	break;
	}
}

void bsa_rc_path(struct bsa *bsa, int i, int r)
{
	char *path = bsa->rc[r]->path;
	strcpy(path, bsa->ca[i]);
	strcat(path, "\\");
	strcat(path, bsa->cb[r]);
	return path;
}

void resources(struct bsa *bsa)
{
	bsa->rc = malloc(sizeof(struct rc *) * Hedr.files);
	bsa->r = malloc(sizeof(int) * Hedr.folders);
	int r = 0;
	for (int i = 0; i < Hedr.folders; i++)
	{
	bsa->r[i] = r;
	for (int j = 0; j < bsa->fld[i].num; j++)
	{
	bsa->rc[r] = malloc(sizeof(struct rc));
	*bsa->rc[r] = (struct rc){bsa, i, j, r, -1, bsa->cb[r], NULL};
	bsa_rc_path(bsa, i, r);
	r++;
	}
	}
}

api struct rc *bsa_find(struct bsa *bsa, const char *p)
{
	char stem[260], name[260];
	FileStem(stem, p, '\\');
	FileName(name, p, '\\');
	if (stem==NULL||name==NULL)
	return;
	int cmp;
	int r;
	for (int i = 0; i < Hedr.folders; i++)
	{
	cmp = strcmp(stem, bsa->ca[i]);
	if (cmp) continue;
	r = bsa->r[i];
	for (int j = 0; j < bsa->fld[i].num; j++)
	{
	cmp = strcmp(name, bsa->cb[r]);
	if (0 == cmp)
	return bsa->rc[r];
	r++;
	}
	}
	return NULL;
}

api void bsa_search(struct bsa *bsa, struct rc *rcs[BSA_MAX_SEARCHES], const char *s, int *num)
{
	char *str;
	int r;
	int n = 0;
	for (int i = 0; i < Hedr.folders; i++)
	{
	r = bsa->r[i];
	for (int j = 0; j < bsa->fld[i].num; j++)
	{
	str = strstr(bsa->cb[r], s);
	if (str!=NULL) {
	rcs[n++] = bsa->rc[r];
	if (n>=BSA_MAX_SEARCHES)
	goto end;
	}
	r++;
	}
	}
	end:
	*num=n;
}

char *bsa_uncompress(struct rc *rc)
{
	char *src = rc->buf;
	uint32_t size = *(uint32_t *)&src[0];
	rc->size -= sizeof(uint32_t);
	src += sizeof(uint32_t);
	char *dest = malloc(size * sizeof(char));
	int ret = uncompress(dest, (uLongf*)&size, src, rc->size);
	cassert(ret == Z_OK, BSA "zlib");
	rc->size = size;
	return dest;
}

#define EMBED_FILE_NAMES 0x100

#define INVERT_COMPRESSED 0x40000000

// todo embed files names fuffed this up real good
api int bsa_read(struct rc *rc) {
	if (rc == NULL)
	return 0;
	if (rc->buf)
	return 1;
	struct bsa *bsa = rc->bsa;
	struct bsa_file *f = &bsa->file[rc->i][rc->j];
	int offset = f->offset;
	int size = f->size;
	if (Hedr.archive_flags & EMBED_FILE_NAMES)
	{
	unsigned char x;
	seek(bsa, offset);
	read(bsa, &x, 1);
	offset += x + 1;
	size -= x + 1;
	}
	rc->size = size;
	rc->buf = malloc(size * sizeof(char));
	seek(bsa, offset);
	read(bsa, rc->buf, rc->size);
	if ((Hedr.archive_flags & 0x4) != (f->size & INVERT_COMPRESSED))
	{
	char *x = bsa_uncompress(rc);
	free(rc->buf);
	rc->buf = x;
	}
	return 1;
}

api void bsa_free(struct bsa **b)
{
	struct bsa *bsa = *b;
	*b = NULL;
	fclose(bsa->stream);
	return;
	// Delete file records
	if (Hedr.folders)
	{
	for (int i = 0; i < Hedr.folders; i++)
	{
	free(bsa->file[i]);
	free(bsa->ca[i]);
	}
	free(bsa->file);
	free(bsa->ca);
	}
	// Delete folder records
	free(bsa->fld);
	free(bsa->cb);
	// Delete filenames
	free(bsa->cb[0]);
	// Other
	free(bsa);
	*b = NULL;
}

// archive ext stuff below

static struct bsa *archives[10];

api struct bsa **get_archives()
{
	return archives;
}

api struct bsa *bsa_get(const char *path)
{
	for (int i = 10; i --> 0; )
	{
	struct bsa *bsa = archives[i];
	if (bsa==NULL)
	continue;
	if (0 == strcmp(path, bsa->path))
	return bsa;
	return NULL;
	}
}

api struct rc *bsa_find_more(const char *p, unsigned long flags)
{
	struct rc *rc = NULL;
	for (int i = 10; i --> 0; )
	{
	struct bsa *bsa = archives[i];
	if (bsa==NULL)
	continue;
	int test = Hedr.file_flags & flags;
	if (Hedr.file_flags == 0 || test)
	rc = bsa_find(bsa, p);
	if (rc!=NULL)
	break;
	}
	return rc;
}

static int read(struct bsa *bsa, void *data, unsigned size)
{
	return fread(data, 1, size, (FILE *)bsa->stream);
}

static int seek(struct bsa *bsa, unsigned offset)
{
	return fseek((FILE *)bsa->stream, offset, SEEK_SET);
}