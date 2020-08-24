#include "c/c.h"

#include "bsa.h"

// based on
// https://github.com/Ortham/libbsa/blob/master/src/api/tes4bsa.cpp

#include <zlib.h>

#define BSA "bsa - "
#define VER 104
#define VER_SE 105

#define QUESTION "is your 'path to oldrim.txt' set correctly?"

static int read(bsa_t *b, void *data, unsigned size)
{
	return fread(data, 1, size, (FILE *)b->stream);
}

static int seek(bsa_t *b, unsigned offset)
{
	return fseek((FILE *)b->stream, offset, SEEK_SET);
}

int NUM = 0;

api bsa_t bsa_load(const char *s)
{
#define hedr bsa.hdr
	bsa_t bsa;
	bsa.num = NUM++;
	//bsa.pos = 0;
	bsa.stream = fopen(s, "rb");
	cassert_(
		bsa.stream, BSA "can't open");
	read(&bsa, &hedr, sizeof(bsa_hedr_t));
	//printf(bsa_print_hedr(&bsa));
	cassert_(
		strcmp(
			"BSA\x00", (char *)&hedr.id) == 0,
		BSA "not a bsa");
	cassert_(
		hedr.ver != VER_SE, BSA "ver 105 is SE");
	cassert_(
		hedr.ver == VER, BSA "not 104");
	bsa_read_folder_records(&bsa);
	bsa_read_file_records(&bsa);
	bsa_read_filenames(&bsa);
	bsa_resources(&bsa);
	log_("loaded bsa");
	bsa.magic = 654;
	return bsa;
}

void bsa_read_folder_records(bsa_t *b)
{
#define hedr b->hdr
	b->fld = malloc(sizeof(bsa_fld_t) * hedr.folders);
	read(b, b->fld, hedr.folders * sizeof(bsa_fld_t));
}

void bsa_read_file_records(bsa_t *b)
{
#define hedr b->hdr
	b->file = malloc(sizeof(bsa_file_t *) * hedr.folders);
	b->ca = malloc(sizeof(char *) * hedr.folders);
	for (int i = 0; i < hedr.folders; i++)
	{
	const int num = b->fld[i].num;
	b->file[i] = malloc(sizeof(bsa_file_t) * num);
	b->ca[i] = bsa_read_bzstring(b);
	read(b, b->file[i], sizeof(bsa_file_t) * num);
	}
}

void bsa_read_filenames(bsa_t *b)
{
#define hedr b->hdr
	char *buf = malloc(sizeof(char) * hedr.filesl);
	b->cb = malloc(sizeof(char *) * hedr.files);
	read(b, buf, hedr.filesl);
	int i = 0, j = 0, n = 0;
	while (i++ < hedr.filesl)
	{
	if (buf[i] != '\0')
		continue;
	b->cb[n++] = &buf[j];
	j = i + 1;
	if (n >= hedr.files)
		break;
	}
}

char *bsa_read_bzstring(bsa_t *b)
{
	int c = 0;
	read(b, &c, 1);
	char *name = malloc(sizeof(char) * c);
	read(b, name, c);
	return name;
}

char *bsa_path(bsa_t *b, int i, int r)
{
	char *path = (char *)malloc(strlen(b->ca[i]) + strlen(b->cb[r]) + 2);
	strcpy(path, b->ca[i]);
	strcat(path, "\\");
	strcat(path, b->cb[r]);
	return path;
}

void bsa_resources(bsa_t *b)
{
#define hedr b->hdr
	b->rc = malloc(sizeof(rc_t *) * hedr.files);
	b->r = malloc(sizeof(int) * hedr.folders);
	int r = 0;
	for (int i = 0; i < hedr.folders; i++)
	{
	b->r[i] = r;
	for (int j = 0; j < b->fld[i].num; j++)
	{
	b->rc[r] = malloc(sizeof(rc_t));
	// todo wobly
	*b->rc[r] = (rc_t){b, i, j, r, b->file[i][j].size, b->cb[r], bsa_path(b, i, r), NULL, NULL};
	r++;
	}
	}
}

api rc_t *bsa_find(bsa_t *b, const char *p)
{
#define hedr b->hdr
	rc_t *rc = NULL;
	char *stem = fstem(p, '\\');
	char *name = fname(p, '\\');
	if (!stem || !name) goto end;
	int cmp;
	int r;
	for (int i = 0; i < hedr.folders; i++)
	{
	cmp = strcmp(stem, b->ca[i]);
	if (cmp) continue;
	r = b->r[i];
	for (int j = 0; j < b->fld[i].num; j++)
	{
	cmp = strcmp(name, b->cb[r]);
	if (!cmp) {
	rc = b->rc[r];
	goto end;
	}
	r++;
	}
	}
	end:
	free(stem);
	free(name);
	return rc;
}

char *bsa_uncompress(char *data, size_t size)
{
	uint32_t u = *(uint32_t *)&data[0];
	printf("bsa uncompress u %u\n", u);
	size -= sizeof(uint32_t);
	data += sizeof(uint32_t);
	char *dest = malloc(u * sizeof(char));
	char *src = data;
	int ret = uncompress(dest, (uLongf*)&u, src, size);
	printf("uncompress ret %i", ret);
	cassert_(ret == Z_OK, BSA "zlib");
	return dest;
}

#define EMBED_FILE_NAMES 0x100
#define INVERT_COMPRESSED 0x40000000

api int bsa_read(bsa_t *b, rc_t *rc) {
	if (!rc || rc->buf || rc->inf)
	return 0;
	bsa_file_t *f = &b->file[rc->i][rc->j];
	unsigned long size = f->size;
	if (hedr.archive_flags & EMBED_FILE_NAMES)
	{
	unsigned char bstring = 0;
	seek(b, f->offset);
	read(b, &bstring, 1);
	size -= bstring + 1;
	seek(b, f->offset + bstring + 1);
	}
	else
	{
	seek(b, f->offset);
	}
	rc->buf = malloc(size * sizeof(char));
	rc->size = size;
	read(b, rc->buf, size);
	printf("bsa compress: %i, rc %s compress: %i\n", hedr.archive_flags & 0x4, b->cb[rc->r], f->size & INVERT_COMPRESSED);
	printf("bsa compress size: %i\n", f->size);
	printf("bsa compress corrected size: %i\n", size);
	if ((hedr.archive_flags & 0x4) != (f->size & INVERT_COMPRESSED))
	{
	rc->inf = bsa_uncompress(rc->buf, f->size);
	}
	return 1;
}