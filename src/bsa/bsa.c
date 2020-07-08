#include "bsa.h"

#include "c/hamburger helper.h"
#include "c/files.h"

#include <stdlib.h>
#include <stdio.h>

#define BSA "bsa - "
#define VER 104
#define VER_SE 105

#define QUESTION "is your 'path to oldrim.txt' set correctly?"

static int read(bsa_t *b, void *data, unsigned size) {
	unsigned res = fread(data, 1, size, (FILE *)b->stream);
	assert_(res == size, "res != size");
}

static int seek(bsa_t *b, unsigned offset) {
	unsigned res = fseek((FILE *)b->stream, offset, SEEK_SET);
	assert_(!res, "!res");
}

api bsa_t bsa_load(const char *s)
{
#define hedr bsa.hdr
	bsa_t bsa;
	//bsa.pos = 0;
	bsa.stream = fopen(s, "rb");
	assert_(
		bsa.stream, BSA "can't open. " QUESTION);
	read(&bsa, &hedr, sizeof(hedr_t));
	assert_(
		strcmp(
			"BSA\x00", (char *)&hedr.id) == 0,
		BSA "not a bsa");
	assert_(
		hedr.ver != VER_SE, BSA "ver 105 is SE");
	assert_(
		hedr.ver == VER, BSA "not 104");
	bsa_read_folder_records(&bsa);
	bsa_read_file_records(&bsa);
	bsa_read_filenames(&bsa);
	bsa_resources(&bsa);
	return bsa;
}

void bsa_read_folder_records(bsa_t *b)
{
#define hedr b->hdr
	b->fld = malloc(sizeof(fld_t) * hedr.folders);
	read(b, b->fld, hedr.folders * sizeof(fld_t));
}

void bsa_read_file_records(bsa_t *b)
{
#define hedr b->hdr
	b->fle = malloc(sizeof(fle_t *) * hedr.folders);
	b->ca = malloc(sizeof(char *) * hedr.folders);
	for (int i = 0; i < hedr.folders; i++)
	{
	const int num = b->fld[i].num;
	b->fle[i] = malloc(sizeof(fle_t) * num);
	b->ca[i] = bsa_read_bzstring(b);
	read(b, b->fle[i], sizeof(fle_t) * num);
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
#define HACK "\\"
	char *path = (char *)malloc(strlen(b->ca[i]) + strlen(b->cb[r]) + 2);
	strcpy(path, b->ca[i]);
	strcat(path, HACK);
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
	//b->rc[r] = {&b, i, j, r, b->cb[r], bsa_path(b, i, r)};
	r++;
	}
	}
}

api rc_t *bsa_find(bsa_t *b, const char *p)
{
#define HACK '\\'
#define hedr b->hdr
	char *stem = fstem(p, HACK);
	char *name = fname(p, HACK);
	if (!stem || !name) goto end;
	int cmp;
	int r;
	rc_t *rc = 0;
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

api void bsa_read(bsa_t *b, rc_t *rc) {
	if (!rc)
		return;
	fle_t *f = &b->fle[rc->i][rc->j];
	rc->buf = (const unsigned char *)malloc(f->size);
	seek(b, f->offset);
	read(b, (void *)rc->buf, f->size);
}
