#include "bsa"

#include "dark2.h"

#include "files"

#include <sstream>

#define BSA "BSA - "

template <typename... Args>
void _write(stringstream &ss, Args &&... args)
{
	(ss << ... << args);
}

void bsa_print_rc(bsa_t &b, stringstream &ss, int r)
{
	rc_t &rc = *b.rc[r];
	_write(
		ss,
		"^", r,
		"\nfilename: ", rc.name,
		"\nfolder: ", b.ca[rc.i],
		"\n");
}

void bsa_print_fle_rcd(bsa_t &b, stringstream &ss, int i, int j)
{
	fle_t &rcd = b.fle[i][j];
	_write(
		ss,
		"^", b.r[i] + j,
		"\nhash: ", rcd.hash,
		"\nsize: ", rcd.size,
		"\noffset: ", rcd.offset,
		"\n");
}

void bsa_print_fld_rcd(bsa_t &b, stringstream &ss, int n)
{
	fld_t &rcd = b.fld[n];
	_write(
		ss,
		"^", n,
		"\nhash: ", rcd.hash,
		"\nnum: ", rcd.num,
		"\noffset: ", rcd.offset,
		"\n");
}

void bsa_print_hedr(bsa_t &b, stringstream &ss)
{
#define hedr b.hdr
	_write(
		ss,
		"id: ", hedr.id,
		"\nver: ", hedr.ver,
		"\noffset: ", hedr.offset,
		"\narchive flags: ", hedr.archive_flags,
		"\n  0x1: ", hedr.archive_flags & 0x1,
		"\n  0x2: ", hedr.archive_flags & 0x2,
		"\n  0x4: ", hedr.archive_flags & 0x4,
		"\n  0x8: ", hedr.archive_flags & 0x8,
		"\n  0x10: ", hedr.archive_flags & 0x10,
		"\n  0x20: ", hedr.archive_flags & 0x20,
		"\n  0x40: ", hedr.archive_flags & 0x40,
		"\n  0x80: ", hedr.archive_flags & 0x80,
		"\n  0x100: ", hedr.archive_flags & 0x100,
		"\n  0x200: ", hedr.archive_flags & 0x200,
		"\n  0x400: ", hedr.archive_flags & 0x400,
		"\nfolders: ", hedr.folders,
		"\nfiles: ", hedr.files,
		"\nfoldersl: ", hedr.foldersl,
		"\nfilesl: ", hedr.filesl,
		"\nfile_flags: ", hedr.file_flags,
		"\nsizeof: ", sizeof(hedr),
		"\n");
}


void bsa_print(bsa_t &b)
{
#if 0
#define hedr b.hdr
	ofstream os("bsa.txt");
	stringstream ss;
	bsa_print_hedr(b, ss);
	int f = 0;
	for (int i = 0; i < hedr.folders; i++)
	{
		fld_t &fld = b.fld[i];
		_write(ss, "\n\n");
		_write(ss, "folder ", i + 1, " of ", hedr.folders, ", ", b.ca[i]);
		_write(ss, "\n");
		bsa_print_fld_rcd(b, ss, i);
		for (int j = 0; j < fld.num; j++)
		{
			_write(ss, "  ", b.cb[f++]);
			bsa_print_fle_rcd(b, ss, i, j);
		}
	}
	os << ss.str();
	os.close();
#endif
}