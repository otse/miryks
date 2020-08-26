#include "c/c.h"

#include "bsa.h"

// based on
// https://github.com/Ortham/libbsa/blob/master/src/api/tes4bsa.cpp

#include <zlib.h>

#define BSA "bsa - "
#define VER 104
#define VER_SE 105

int read(bsa_t *b, void *data, unsigned size) {
	return fread(data, 1, size, (FILE *)b->stream);
}

int seek(bsa_t *b, unsigned offset) {
	return fseek((FILE *)b->stream, offset, SEEK_SET);
}

bsas_t bsas;
int NUM = 0;

void bsa_read_folder_records(bsa_t *);
void bsa_read_file_records(bsa_t *);
void bsa_read_filenames(bsa_t *);
void bsa_resources(bsa_t *);
void bsa_bsort(bsa_t *);

api bsa_t* bsa_load(const char *path)
{
#define hedr bsa->hdr
	bsa_t *bsa = malloc(sizeof(bsa_t));
	memset(bsa, 0, sizeof(bsa_t));
	bsa->num = NUM++;
	bsa->path = malloc(sizeof(char) * strlen(path) + 1);
	strcpy(bsa->path, path);
	bsa->stream = fopen(path, "rb");
	cassert_(
		bsa->stream, BSA "can't open");
	read(bsa, &hedr, sizeof(bsa_hedr_t));
	//printf(bsa_print_hedr(&bsa));
	cassert_(
		strcmp(
			"BSA\x00", (char *)&hedr.id) == 0,
		BSA "not a bsa");
	cassert_(
		hedr.ver != VER_SE, BSA "cant use special edition");
	cassert_(
		hedr.ver == VER, BSA "not 104");
	bsa_read_folder_records(bsa);
	bsa_read_file_records(bsa);
	bsa_read_filenames(bsa);
	bsa_resources(bsa);
	printf("loaded bsa %s\n", path);
	return bsa;
}

api void bsas_add_to_loaded(bsas_t *bsas, bsa_t **bsa, int num)
{
	for (int i = 0; i < num; i++)
	{
	bsas->bsas[bsas->num++] = &bsa[i];
	}
}

api bsa_t *bsas_get_by_path(bsas_t *bsas, const char *path)
{

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

void bsa_rc_path(bsa_t *b, int i, int r)
{
	char *path = b->rc[r]->path;
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
	*b->rc[r] = (rc_t){b, i, j, r, -1, b->cb[r], 0, NULL};
	bsa_rc_path(b, i, r);
	r++;
	}
	}
}

api rc_t *bsa_find(bsa_t *b, const char *p)
{
#define hedr b->hdr
	rc_t *rc = NULL;
	char *stem = FileStem(p, '\\');
	char *name = FileName(p, '\\');
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

api rc_t *bsas_find(bsas_t *bsas, const char *p)
{
	
}

char *bsa_uncompress(rc_t *rc)
{
	char *src = rc->buf;
	uint32_t size = *(uint32_t *)&src[0];
	rc->size -= sizeof(uint32_t);
	src += sizeof(uint32_t);
	char *dest = malloc(size * sizeof(char));
	int ret = uncompress(dest, (uLongf*)&size, src, rc->size);
	cassert_(ret == Z_OK, BSA "zlib");
	rc->size = size;
	return dest;
}

#define EMBED_FILE_NAMES 0x100

#define INVERT_COMPRESSED 0x40000000


api int bsa_read_rc(rc_t *rc) {
	if (rc == NULL)
	return 0;
	if (rc->buf)
	return 1;
	bsa_t *b = rc->b;
	bsa_file_t *f = &b->file[rc->i][rc->j];
	int offset = f->offset;
	int size = f->size;
	if (hedr.archive_flags & EMBED_FILE_NAMES)
	{
	unsigned char x;
	seek(b, offset);
	read(b, &x, 1);
	offset += x + 1;
	size -= x + 1;
	}
	rc->size = size;
	rc->buf = malloc(size * sizeof(char));
	seek(b, offset);
	read(b, rc->buf, rc->size);
	if ((hedr.archive_flags & 0x4) != (f->size & INVERT_COMPRESSED))
	{
	char *x = bsa_uncompress(rc);
	free(rc->buf);
	rc->buf = x;
	}
	return 1;
}

api void bsa_free(bsa_t *b)
{
#define hedr b->hdr
	// Delete file records
	if (hedr.folders)
	{
	for (int i = 0; i < hedr.folders; i++)
	{
	free(b->file[i]);
	free(b->ca[i]);
	}
	free(b->file);
	free(b->ca);
	}
	// Delete folder records
	free(b->fld);
	free(b->cb);
	// Delete filenames
	if (hedr.files)
	free(b->cb[0]);
	// Other
	for (int i = 0; i < hedr.folders; i++)
	{
	
	}
	free(b);
	b = NULL;
}