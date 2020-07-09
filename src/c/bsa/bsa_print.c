#include "c/c.h"

#include "bsa.h"
#include "../files.h"

char *bsa_print_rc(bsa_t *b, int r)
{
	rc_t *rc = b->rc[r];
	char *buf = malloc(sizeof(char) * 200);
	int w = snprintf(
		buf, 200,
"\
^%i\
\nfilename: %s\
\nfolder: %s",
		r,
		rc->name,
		b->ca[rc->i]);
	return buf;
}

char *bsa_print_fle_rcd(bsa_t *b, int i, int j)
{
	fle_t *rcd = &b->fle[i][j];
	char *buf = malloc(sizeof(char) * 100);
	int w = snprintf(
		buf, 100,
"\
^%i\
\nhash: %llu\
\nsize: %lu\
\noffset: %lu",
		b->r[i] + j,
		rcd->hash,
		rcd->size,
		rcd->offset);
	return buf;
}

char *bsa_print_fld_rcd(bsa_t *b, int n)
{
	fld_t *rcd = &b->fld[n];
	char *buf = malloc(sizeof(char) * 200);
	int w = snprintf(
		buf, 200,
"\
^%i\
\nhash: %llu\
\nnum: %lu\
\noffset: %lu",
		n, rcd->hash,
		rcd->num,
		rcd->offset);
	return buf;
}

char *bsa_print_hedr(bsa_t *b)
{
#define hedr b->hdr
	char *buf = malloc(sizeof(char) * 500);
	int w = snprintf(
		buf, 500,
"\
id: %s\
\nver: %i\
\noffset: %lu\
\narchive flags:\
\n 0x1: %i\
\n 0x2: %i\
\n 0x4: %i\
\n 0x8: %i\
\n 0x10: %i\
\n 0x20: %i\
\n 0x40: %i\
\n 0x80: %i\
\n 0x100: %i\
\n 0x200: %i\
\n 0x400: %i\
\nfolders: %lu\
\nfiles: %lu\
\nfoldersl: %lu\
\nfilesl: %lu\
\nfile_flags: %lu\
\nsizeof: %zu",
		hedr.id,
		hedr.ver,
		hedr.archive_flags,
		hedr.archive_flags & 0x1,
		hedr.archive_flags & 0x2,
		hedr.archive_flags & 0x4,
		hedr.archive_flags & 0x8,
		hedr.archive_flags & 0x10,
		hedr.archive_flags & 0x20,
		hedr.archive_flags & 0x40,
		hedr.archive_flags & 0x80,
		hedr.archive_flags & 0x100,
		hedr.archive_flags & 0x200,
		hedr.archive_flags & 0x400,
		hedr.folders,
		hedr.files,
		hedr.foldersl,
		hedr.filesl,
		hedr.file_flags,
		sizeof(bsa_hedr_t)
		);
	return buf;
}

void bsa_print(bsa_t *b)
{
#if 0
#define hedr b->hdr
	ofstream os("bsa.txt");
	stringstream ss;
	bsa_print_hedr(b, ss);
	int f = 0;
	for (int i = 0; i < hedr.folders; i++)
	{
		fld_t &fld = b->fld[i];
		_write(ss, "\n\n");
		_write(ss, "folder ", i + 1, " of ", hedr.folders, ", ", b->ca[i]);
		_write(ss, "\n");
		bsa_print_fld_rcd(b, ss, i);
		for (int j = 0; j < fld.num; j++)
		{
			_write(ss, "  ", b->cb[f++]);
			bsa_print_fle_rcd(b, ss, i, j); 
		}
	}
	os << ss.str();
	os.close();
#endif
}