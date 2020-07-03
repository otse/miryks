#include "bsa"

#include "dark2.h"

#include "files.hpp"

#define BSA "BSA - "

ofstream *txt;

template <typename... Args>
void _write(
	Args &&... args)
{
	(*txt << ... << args) << std::endl;
}

void bsa_print_file_rcd(bsa_t &b, int i, int j)
{
	fle_t &rcd = b.files[i][j];
	_write(
		"  --file rcd ", i, ", ", j, "--",
		"\n   long long nameHash: ", rcd.nameHash,
		"\n   size: ", rcd.size,
		"\n   offset: ", rcd.offset,
		"\n  --");
}

void bsa_print_fld_rcd(bsa_t &b, int n)
{
	fld_t &rcd = b.folds[n];
	_write(
		"--folder rcd ", n, "--",
		"\n long long hash: ", rcd.hash,
		"\n num: ", rcd.num,
		"\n offset: ", rcd.offset,
		"\n--");
}

void bsa_print_hedr(bsa_t &b)
{
#define hedr b.hdr
	_write(
		"--hedr--",
		"\n id: ", hedr.id,
		"\n ver: ", hedr.ver,
		"\n offset: ", hedr.offset,
		"\n archive flags: ", hedr.archive_flags,
		"\n   0x1: ", hedr.archive_flags & 0x1,
		"\n   0x2: ", hedr.archive_flags & 0x2,
		"\n   0x4: ", hedr.archive_flags & 0x4,
		"\n   0x8: ", hedr.archive_flags & 0x8,
		"\n   0x10: ", hedr.archive_flags & 0x10,
		"\n   0x20: ", hedr.archive_flags & 0x20,
		"\n   0x40: ", hedr.archive_flags & 0x40,
		"\n   0x80: ", hedr.archive_flags & 0x80,
		"\n   0x100: ", hedr.archive_flags & 0x100,
		"\n   0x200: ", hedr.archive_flags & 0x200,
		"\n   0x400: ", hedr.archive_flags & 0x400,
		"\n fos: ", hedr.fos,
		"\n fis: ", hedr.fis,
		"\n fol: ", hedr.fol,
		"\n fil: ", hedr.fil,
		"\n file_flags: ", hedr.file_flags,
		"\n sizeof: ", sizeof(hedr),
		"\n--");
}

void bsa_print(bsa_t &b)
{
#define hedr b.hdr
	ofstream os("bsa.txt");
	txt = &os;
	bsa_print_hedr(b);
	int f = 0;
	for (int i = 0; i < hedr.fos; i++)
	{
		fld_t &fld = b.folds[i];
		_write("\n\n");
		_write("folder name ", b.charsf[i]);
		bsa_print_fld_rcd(b, i);
		for (int j = 0; j < fld.num; j++)
		{
			_write("  ", b.charsff[f++]);
			bsa_print_file_rcd(b, i, j);
		}
	}
	os.close();
}