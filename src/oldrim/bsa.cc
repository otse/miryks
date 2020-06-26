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
			"BSA\x00", &hedr.id[0]) == 0,
		BSA "not a bsa");
	assert_(
		hedr.ver != VER_SE, BSA "this is SE");
	assert_(
		hedr.ver == VER, BSA "not 104");
	bsa_print_hedr(bsa);
	bsa_read_folder_records(bsa);
	bsa_read_file_records(bsa);
	bsa_read_file_names(bsa);
	bsa_produce_assets(bsa);
	return bsa;
}

void bsa_read_folder_records(bsa_t &b)
{
#define hedr b.hdr
	b.flds =
		new fld_rcd_t[hedr.fos];
	b.is.read(
		(char *)b.flds, hedr.fos * sizeof(fld_rcd_t));
	int n = 0;
	log_("end of flds ", b.is.tellg());
}

void bsa_read_file_records(bsa_t &b)
{
#define hedr b.hdr
	log_("file rcd begin ", b.is.tellg());
	auto start = b.is.tellg();
	b.files =
		new file_rcd_t *[hedr.fos];
	for (int i = 0; i < hedr.fos; i++)
	{
		fld_t fld;
		const int num = b.flds[i].num;
		b.files[i] = new file_rcd_t[num];
		getline(
			b.is, fld.name, '\0');
		b.is.read(
			(char *)b.files[i], sizeof(file_rcd_t) * num);
		fld.files = b.files[i];
		b.folders.push_back(fld);
		log_("files name: ", fld.name);
		log_("size ", b.is.tellg() - start);
		bsa_print_fld_rcd(b, i);
		bsa_print_file_rcd(b, i);
	}
}

void bsa_read_file_names(bsa_t &b)
{
#define hedr b.hdr
	b.finames = new char[hedr.fil];
	b.is.read((char *)b.finames, hedr.fil);
	log_("finames: ", b.finames);
}

void bsa_print_file_rcd(bsa_t &bsa, int n)
{
	file_rcd_t &rcd = bsa.files[n][0];
	log_(
		"--file rcd ", n, "--",
		"\n long long nameHash: ", rcd.nameHash,
		"\n size: ", rcd.size,
		"\n offset: ", rcd.offset,
		"\n--");
}

void bsa_print_fld_rcd(bsa_t &bsa, int n)
{
	fld_rcd_t &rcd = bsa.flds[n];
	log_(
		"--folder rcd ", n, "--",
		"\n long long hash: ", rcd.hash,
		"\n num: ", rcd.num,
		"\n offset: ", rcd.offset,
		"\n--");
}

void bsa_print_hedr(bsa_t &bsa)
{
#define hedr bsa.hdr
	log_(
		"--hedr--",
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

void bsa_produce_assets(bsa_t &bsa)
{
	return;
	int l = 0;
	int n = 0;
	for (; n < hedr.fis; n++)
	{
		string s(bsa.finames + l);
		l += s.length() + 1;
		log_("file_t ", s);
		bsa.assets.push_back(asset_t{s});
	}
}