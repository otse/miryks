#include "common.h"

#include "bsa.h"

// based on Ortham/libbsa

#if BSA_VER==105
#include <lz4frame_static.h>
#else
#include <zlib.h>
#endif

#define Hedr bsa->hdr

static int read(Bsa *, void *, unsigned);
static int seek(Bsa *, unsigned);

char *bsa_read_bzstring(Bsa *);

void read_folder_records(Bsa *);
void read_file_records(Bsa *);
void read_filenames(Bsa *);
void resources(Bsa *);

api Bsa *bsa_load(const char *path)
{
	Bsa *bsa = malloc(sizeof(Bsa));
	memset(bsa, 0, sizeof(Bsa));
	bsa->path = malloc(sizeof(char) * strlen(path) + 1);
	file_name(bsa->filename, path, '/');
	strcpy(bsa->path, path);
	bsa->stream = fopen(path, "rb");
	assertm(
		bsa->stream, path);
	read(bsa, &Hedr, sizeof(struct bsa_hedr));
	assertm(
		strcmp(
			"BSA\x00", (char *)&Hedr.id) == 0,
		"bsa - " "not a bsa");
	assertm(
		Hedr.ver == BSA_VER, "bsa - " "wrong version");
#if 0
	printf("\
	header_flags\n\
	\n0x1   %i\n0x2   %i\\n0x4   %i\\n0x8   %i\\n0x10  %i\\n0x20  %i\\n0x40  %i\\n0x80  %i\\n0x100 %i\\n0x200 %i\\n\
	",
	Hedr.archive_flags & 0x1,
	Hedr.archive_flags & 0x2,
	Hedr.archive_flags & 0x4,
	Hedr.archive_flags & 0x8,
	Hedr.archive_flags & 0x10,
	Hedr.archive_flags & 0x20,
	Hedr.archive_flags & 0x40,
	Hedr.archive_flags & 0x80,
	Hedr.archive_flags & 0x100,
	Hedr.archive_flags & 0x200
	);
#endif
	read_folder_records(bsa);
	read_file_records(bsa);
	read_filenames(bsa);
	resources(bsa);
	return bsa;
}

void read_folder_records(Bsa *bsa)
{
	bsa->fld = malloc(sizeof(struct bsa_fld) * Hedr.folders);
	read(bsa, bsa->fld, Hedr.folders * sizeof(struct bsa_fld));
}

char *bsa_read_bzstring(Bsa *bsa)
{
	int c = 0;
	read(bsa, &c, 1);
	char *name = malloc(sizeof(char) * c);
	read(bsa, name, c);
	return name;
} 

void read_file_records(Bsa *bsa)
{
	bsa->file = malloc(sizeof(struct bsa_file *) * Hedr.folders);
	bsa->ca = malloc(sizeof(char *) * Hedr.folders);
	for (unsigned int i = 0; i < Hedr.folders; i++)	
	{
	const int num = bsa->fld[i].num;
	bsa->file[i] = malloc(sizeof(struct bsa_file) * num);
	bsa->ca[i] = bsa_read_bzstring(bsa);
	read(bsa, bsa->file[i], sizeof(struct bsa_file) * num);
	}
}

void read_filenames(Bsa *bsa)
{
	char *buf = malloc(sizeof(char) * Hedr.filesl);
	bsa->cb = malloc(sizeof(char *) * Hedr.files);
	read(bsa, buf, Hedr.filesl);
	unsigned int i = 0, j = 0, n = 0;
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

void bsa_rc_path(Bsa *bsa, int i, int r)
{
	char *path = bsa->rc[r]->path;
	strcpy(path, bsa->ca[i]);
	strcat(path, "\\");
	strcat(path, bsa->cb[r]);
	//return path;
}

void resources(Bsa *bsa)
{
	bsa->rc = malloc(sizeof(Rc *) * Hedr.files);
	bsa->r = malloc(sizeof(int) * Hedr.folders);
	int r = 0;
	for (unsigned int i = 0; i < Hedr.folders; i++)
	{
	bsa->r[i] = r;
	for (unsigned int j = 0; j < bsa->fld[i].num; j++)
	{
	bsa->rc[r] = malloc(sizeof(Rc));
	*bsa->rc[r] = (Rc){bsa, i, j, r, -1, bsa->cb[r], NULL};
	bsa_rc_path(bsa, i, r);
	r++;
	}
	}
}

api Rc *bsa_find(Bsa *bsa, const char *p)
{
	char stem[260], name[260];
	file_stem(stem, p, '\\');
	file_name(name, p, '\\');
	if (stem==NULL||name==NULL)
	return NULL;
	int cmp;
	int r;
	for (unsigned int i = 0; i < Hedr.folders; i++)
	{
	cmp = strcmp(stem, bsa->ca[i]);
	if (cmp) continue;
	r = bsa->r[i];
	for (unsigned int j = 0; j < bsa->fld[i].num; j++)
	{
	cmp = strcmp(name, bsa->cb[r]);
	if (0 == cmp)
	return bsa->rc[r];
	r++;
	}
	}
	return NULL;
}

api void bsa_search(Bsa *bsa, Rc *rcs[BSA_MAX_SEARCHES], const char *s, int *num)
{
	char *str;
	int r;
	int n = 0;
	for (unsigned int i = 0; i < Hedr.folders; i++)
	{
	r = bsa->r[i];
	for (unsigned int j = 0; j < bsa->fld[i].num; j++)
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

#if BSA_VER==104

char *bsa_uncompress(Rc *rc)
{
	// zlib
	char *src = rc->buf;
	uint32_t size = *(uint32_t *)&src[0];
	rc->size -= sizeof(uint32_t);
	src += sizeof(uint32_t);
	char *dest = malloc(size * sizeof(char));
	int ret = uncompress(dest, (uLongf*)&size, src, rc->size);
	assertm(ret == Z_OK, BSA "zlib");
	rc->size = size;
	return dest;
}

#elif BSA_VER==105

char *bsa_uncompress(Rc *rc)
{
	//printf("uncompress!\n");
	char *src = rc->buf;
	uint32_t size = *(uint32_t *)&src[0];
	rc->size -= sizeof(uint32_t);
	src += sizeof(uint32_t);
	char *dest = malloc(size * sizeof(char));
	LZ4F_decompressionContext_t context = NULL;
	LZ4F_createDecompressionContext(&context, LZ4F_VERSION);
	LZ4F_decompressOptions_t options;
	LZ4F_errorCode_t errorCode = LZ4F_decompress(
		context, dest, &size, src, &rc->size, &options);
	rc->size = size;
	return dest;
}

#endif

#define EMBED_FILE_NAMES 0x100

#define COMPRESSED_ARCHIVE 0x4

#define INVERT_COMPRESSED 0x40000000

api int bsa_read(Rc *rc) {
	if (rc == NULL)
	return 0;
	if (rc->buf)
	return 1;
	Bsa *bsa = rc->bsa;
	struct bsa_file *f = &bsa->file[rc->i][rc->j];
	int offset = f->offset;
	int size = f->size;
	if (Hedr.archive_flags & EMBED_FILE_NAMES)
	{
	// bstring
	unsigned char x;
	seek(bsa, offset);
	read(bsa, &x, 1);
	offset += x + 1;
	size -= x + 1;
	}
	rc->size = size;
	rc->buf = malloc(rc->size);
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

api void bsa_free(Bsa **b)
{
	Bsa *bsa = *b;
	*b = NULL;
	fclose(bsa->stream);
	return;
	// Delete file records
	if (Hedr.folders)
	{
	for (unsigned int i = 0; i < Hedr.folders; i++)
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

static Bsa *archives[20] = { NULL };

api Bsa **get_archives()
{
	return archives;
}

api Bsa *bsa_get(const char *filename)
{
	for (int i = 20; i --> 0; )
	{
	Bsa *bsa = archives[i];
	if (bsa==NULL)
	continue;
	if (bsa->filename==NULL)
	continue;
	if (0 == strcmp(filename, bsa->filename))
	return bsa;
	}
	return NULL;
}

api Rc *bsa_find_more(const char *p, unsigned long flags)
{
	Rc *rc = NULL;
	for (int i = 20; i --> 0; )
	{
	Bsa *bsa = archives[i];
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

static int read(Bsa *bsa, void *data, unsigned size)
{
	return fread(data, 1, size, (FILE *)bsa->stream);
}

static int seek(Bsa *bsa, unsigned offset)
{
	return fseek((FILE *)bsa->stream, offset, SEEK_SET);
}