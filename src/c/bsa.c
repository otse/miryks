#include "c/c.h"

#include "bsa.h"

// based on
// https://github.com/Ortham/libbsa/blob/master/src/api/tes4bsa.cpp

#include <zlib.h>

#define BSA "bsa - "
#define VER 104
#define VER_SE 105

int read(struct bsa *, void *, unsigned);
int seek(struct bsa *, unsigned);

char *bsa_read_bzstring(struct bsa *);

void bsa_read_folder_records(struct bsa *);
void bsa_read_file_records(struct bsa *);
void bsa_read_filenames(struct bsa *);
void bsa_resources(struct bsa *);
void bsa_bsort(struct bsa *);

api struct bsa *bsa_load(const char *path)
{
#define hedr bsa->hdr
	struct bsa *bsa = malloc(sizeof(struct bsa));
	memset(bsa, 0, sizeof(struct bsa));
	bsa->path = malloc(sizeof(char) * strlen(path) + 1);
	strcpy(bsa->path, path);
	bsa->stream = fopen(path, "rb");
	cassert_(
		bsa->stream, BSA "can't open");
	read(bsa, &hedr, sizeof(struct bsa_hedr));
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

void bsa_read_folder_records(struct bsa *b)
{
#define hedr b->hdr
	b->fld = malloc(sizeof(struct bsa_fld) * hedr.folders);
	read(b, b->fld, hedr.folders * sizeof(struct bsa_fld));
}

char *bsa_read_bzstring(struct bsa *b)
{
	int c = 0;
	read(b, &c, 1);
	char *name = malloc(sizeof(char) * c);
	read(b, name, c);
	return name;
}

void bsa_read_file_records(struct bsa *b)
{
#define hedr b->hdr
	b->file = malloc(sizeof(struct bsa_file *) * hedr.folders);
	b->ca = malloc(sizeof(char *) * hedr.folders);
	for (int i = 0; i < hedr.folders; i++)
	{
	const int num = b->fld[i].num;
	b->file[i] = malloc(sizeof(struct bsa_file) * num);
	b->ca[i] = bsa_read_bzstring(b);
	read(b, b->file[i], sizeof(struct bsa_file) * num);
	}
}

void bsa_read_filenames(struct bsa *b)
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

void bsa_rc_path(struct bsa *b, int i, int r)
{
	char *path = b->rc[r]->path;
	strcpy(path, b->ca[i]);
	strcat(path, "\\");
	strcat(path, b->cb[r]);
	return path;
}

void bsa_resources(struct bsa *b)
{
#define hedr b->hdr
	b->rc = malloc(sizeof(struct rc *) * hedr.files);
	b->r = malloc(sizeof(int) * hedr.folders);
	int r = 0;
	for (int i = 0; i < hedr.folders; i++)
	{
	b->r[i] = r;
	for (int j = 0; j < b->fld[i].num; j++)
	{
	b->rc[r] = malloc(sizeof(struct rc));
	*b->rc[r] = (struct rc){b, i, j, r, -1, b->cb[r], NULL};
	bsa_rc_path(b, i, r);
	r++;
	}
	}
}

api struct rc *bsa_find(struct bsa *b, const char *p)
{
#define hedr b->hdr
	struct rc *rc = NULL;
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

const max_results = 10;

api void bsa_search(struct bsa *b, struct rc *rcs[10], const char *s, int *num)
{
	char *str;
	int r;
	int n = 0;
	for (int i = 0; i < hedr.folders; i++)
	{
	r = b->r[i];
	for (int j = 0; j < b->fld[i].num; j++)
	{
	str = strstr(b->cb[r], s);
	if (str!=NULL) {
	rcs[n++] = b->rc[r];
	if (n>=max_results)
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
	cassert_(ret == Z_OK, BSA "zlib");
	rc->size = size;
	return dest;
}

#define EMBED_FILE_NAMES 0x100

#define INVERT_COMPRESSED 0x40000000

api int bsa_read(struct rc *rc) {
	if (rc == NULL)
	return 0;
	if (rc->buf)
	return 1;
	struct bsa *b = rc->b;
	struct bsa_file *f = &b->file[rc->i][rc->j];
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

api void bsa_free(struct bsa **bsa)
{
	struct bsa *b = *bsa;
	*bsa = NULL;
	return;
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
	free(b->cb[0]);
	// Other
	free(b);
	*bsa = NULL;
}

struct bsas bsas;

api void bsas_add_to_loaded(struct bsas *bsas, struct bsa **bsa, int num)
{
	for (int i = 0; i < num; i++)
	{
	bsas->array[bsas->num++] = bsa[i];
	bsa[i]->bsas = bsas;
	}
}

api struct bsa *bsas_get_by_path(struct bsas *bsas, const char *path)
{
	for (int i = 0; i < bsas->num; i++)
	{
	struct bsa *b = bsas->array[i];
	if (0 == strcmp(b->path, path))
	return b;
	}
	return NULL;
}

api struct rc *bsas_find(struct bsas *bsas, const char *p, unsigned long flags)
{
	struct rc *rc = NULL;
	for (int i = bsas->num; i --> 0; )
	{
	struct bsa *b = bsas->array[i];
	int test = hedr.file_flags & flags;
	if (hedr.file_flags == 0 || test)
	rc = bsa_find(b, p);
	if (rc != NULL)
	break;
	}
	return rc;
}

int read(struct bsa *b, void *data, unsigned size)
{
	return fread(data, 1, size, (FILE *)b->stream);
}

int seek(struct bsa *b, unsigned offset)
{
	return fseek((FILE *)b->stream, offset, SEEK_SET);
}