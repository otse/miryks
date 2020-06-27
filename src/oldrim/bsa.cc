#include "bsa"

#include "dark2.h"

#include "files.hpp"

#define BSA "BSA - "
#define VER 104
#define VER_SE 105

bsa_t bsa_load(const string &a)
{
#define hedr bsa.hdr
	bsa_t bsa;
	bsa.is = ifstream(a, ifstream::binary);
	assert_(
		bsa.is, BSA "cant open");
	bsa.is.read(
		(char *)&hedr, sizeof(bsa_hedr_t));
	assert_(
		strcmp(
			"BSA\x00", (char *)&hedr.id) == 0,
		BSA "not a bsa");
	assert_(
		hedr.ver != VER_SE, BSA "ver 105 is se");
	assert_(
		hedr.ver == VER, BSA "not 104");
	bsa_print_hedr(bsa);
	bsa_read_folder_records(bsa);
	bsa_read_file_records(bsa);
	bsa_read_filenames(bsa);
	bsa_produce_assets(bsa);
	bsa_print_tree(bsa);
	return bsa;
}

void bsa_read_folder_records(bsa_t &b)
{
#define hedr b.hdr
	b._flds =
		new fld_rcd_t[hedr.fos];
	b.is.read(
		(char *)b._flds, hedr.fos * sizeof(fld_rcd_t));
}

void bsa_read_file_records(bsa_t &b)
{
#define hedr b.hdr
	b._files =
		new file_rcd_t *[hedr.fos];
	for (int i = 0; i < hedr.fos; i++)
	{
		fld_t fld;
		fld.rcd = &b._flds[i];
		const int num = fld.rcd->num;
		b._files[i] =
			new file_rcd_t[num];
		b.is.ignore(1); // why?
		getline(
			b.is, fld.name, '\0');
		b.is.read(
			(char *)b._files[i], sizeof(file_rcd_t) * num);
		b.flds.push_back(fld);
	}
}

void bsa_read_filenames(bsa_t &b)
{
#define hedr b.hdr
	char *buf = new char[hedr.fil];
	b.is.read(buf, hedr.fil);
	int pos = 0;
	while (pos < hedr.fil)
	{
		string s = string(buf + pos);
		b.filenames.push_back(s);
		pos += s.length() + 1;
	}
}

const char *bsa_get_filename(bsa_t &b, int n)
{
	return 0;
}

void bsa_print_tree(bsa_t &b)
{
#define hedr b.hdr

	int f = 0;
	for (int i = 0; i < hedr.fos; i++)
	{
		fld_t &fld = b.flds[i];
		log_("folder name ", fld.name);
		bsa_print_fld_rcd(b, i);
		for (int j = 0; j < fld.rcd->num; j++)
		{
			log_("  ", b.filenames[f++]);
			bsa_print_file_rcd(b, i, j);
		}
	}
}

void bsa_print_file_rcd(bsa_t &b, int i, int j)
{
	file_rcd_t &rcd = b._files[i][j];
	log_(
		"  --file rcd ", i, ", ", j, "--",
		"\n   long long nameHash: ", rcd.nameHash,
		"\n   size: ", rcd.size,
		"\n   offset: ", rcd.offset,
		"\n  --");
}

void bsa_print_fld_rcd(bsa_t &b, int n)
{
	fld_rcd_t &rcd = b._flds[n];
	log_(
		"--folder rcd ", n, "--",
		"\n long long hash: ", rcd.hash,
		"\n num: ", rcd.num,
		"\n offset: ", rcd.offset,
		"\n--");
}

void bsa_print_hedr(bsa_t &b)
{
#define hedr b.hdr
	log_(
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

void bsa_produce_assets(bsa_t &b)
{
	/*return;
	int l = 0;
	int n = 0;
	for (; n < hedr.fis; n++)
	{
		string s(b.finames + l);
		l += s.length() + 1;
		log_("file_t ", s);
		b.assets.push_back(asset_t{s});
	}*/
}