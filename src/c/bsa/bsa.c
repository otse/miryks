#include "c/c.h"

#include "bsa.h"

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

api bsa_t bsa_load(const char *s)
{
#define hedr bsa.hdr
	bsa_t bsa;
	//bsa.pos = 0;
	bsa.stream = fopen(s, "rb");
	cassert_(
		bsa.stream, BSA "can't open");
	read(&bsa, &hedr, sizeof(bsa_hedr_t));
	printf(bsa_print_hedr(&bsa));
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
	b->fle = malloc(sizeof(bsa_fle_t *) * hedr.folders);
	b->ca = malloc(sizeof(char *) * hedr.folders);
	for (int i = 0; i < hedr.folders; i++)
	{
	const int num = b->fld[i].num;
	b->fle[i] = malloc(sizeof(bsa_fle_t) * num);
	b->ca[i] = bsa_read_bzstring(b);
	read(b, b->fle[i], sizeof(bsa_fle_t) * num);
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
	*b->rc[r] = (rc_t){b, i, j, r, b->cb[r], bsa_path(b, i, r)};
	r++;
	}
	}
}

api rc_t *bsa_find(bsa_t *b, const char *p)
{
#undef HACK
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

static int zinf(const void *, int, void *, int);

char *decomp(char *src, int size)
{
	void *adr = &src[size - 4];
	unsigned int len = *((unsigned int *)adr);
	printf("size %i len %u\n", size, len);
	char *dest = malloc(len * sizeof(char));
	int ret = zinf(src, size, dest, len);
	printf("ret %i\n", ret);
	cassert_(ret == Z_OK, "decomp");
	return dest;
}

api int bsa_read(bsa_t *b, rc_t *rc) {
	if (!rc)
		return 0;
	bsa_fle_t *f = &b->fle[rc->i][rc->j];
	char *buf, *dest;
	dest = buf = malloc(sizeof(char) * f->size);
	seek(b, f->offset);
	read(b, (void *)buf, f->size);
	if (hedr.archive_flags & 0x4) {
		printf("deflate this\n");
		dest = decomp(buf, f->size);
	}
	rc->buf = dest;
	return 1;
}

static int zinf(const void *src, int srcLen, void *dst, int dstLen)
{
	z_stream strm = {0};
	strm.total_in = strm.avail_in = srcLen;
	strm.total_out = strm.avail_out = dstLen;
	strm.next_in = (Bytef *)src;
	strm.next_out = (Bytef *)dst;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	int err = -1;
	int ret = -1;

	ret = inflateInit2(&strm, -MAX_WBITS);
	if (err == Z_OK)
	{
		err = inflate(&strm, Z_FINISH);
		if (err == Z_STREAM_END)
		{
			ret = strm.total_out;
		}
		else
		{
			inflateEnd(&strm);
			return err;
		}
	}
	else
	{
		inflateEnd(&strm);
		return err;
	}

	inflateEnd(&strm);
	return ret;
}