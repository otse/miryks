#include "bsa"

#include "dark2.h"

#include "files.hpp"

#define BSA "BSA - "
#define VER 104

#define hedr bsa.hedr

bsa_t bsa_load(const string &a)
{
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
		hedr.ver == VER, BSA "not 104");
	bsa_print_hedr(bsa);
	bsa_read_folder_records(bsa);
	bsa_read_file_records(bsa);
	bsa_read_file_names(bsa);
	bsa_produce_fofi(bsa);
	return bsa;
}

void bsa_read_folder_records(bsa_t &bsa)
{
	bsa.forcds = new forcd_t[hedr.fos];
	bsa.is.read(
		(char *)bsa.forcds, hedr.fos * sizeof(forcd_t));
	int n = 0;
	for (; n < hedr.fos; n++)
		bsa_print_forcd(bsa, n);
}

void bsa_read_file_records(bsa_t &bsa)
{
	auto start = bsa.is.tellg();
	int size = sizeof(fircd_t) * hedr.fis;
	size += hedr.fos + hedr.fosl;
	bsa.fircds = new fircd_t[size];
	int n = 0;
	for (; n < hedr.fos; n++)
	{
		string s;
		char ch;
		while ((ch = bsa.is.get()) != '\0')
		{
			s += ch;
		}
		log_("read ", s);
		bsa.is.read(
			(char *)bsa.fircds, sizeof(fircd_t) * bsa.forcds[n].fcount);
		log_("fblock was length ", bsa.is.tellg() - start);
	}
	//int n = 0;
	//for (; n < hedr.fis; n++)
	//bsa_print_fircd(bsa, n);
}

void bsa_read_file_names(bsa_t &bsa)
{
	bsa.finames = new char[hedr.fisl];
	bsa.is.read((char *)bsa.finames, hedr.fisl);
	log_("finames: ", bsa.finames);
}

void bsa_print_fircd(bsa_t &bsa, int n)
{
	fircd_t &rcd = bsa.fircds[n];
	log_(
		"--file rcd ", n, "--",
		"\n long long nameHash: ", rcd.nameHash,
		"\n size: ", rcd.size,
		"\n offset: ", rcd.offset,
		"\n--");
}

void bsa_print_forcd(bsa_t &bsa, int n)
{
	forcd_t &rcd = bsa.forcds[n];
	log_(
		"--folder rcd ", n, "--",
		"\n long long hash: ", rcd.hash,
		"\n fcount: ", rcd.fcount,
		"\n offset: ", rcd.offset,
		"\n--");
}

void bsa_print_hedr(bsa_t &bsa)
{
	log_(
		"--hedr--",
		"\n ver: ", hedr.ver,
		"\n offset: ", hedr.offset,
		"\n flags: ", hedr.flags,
		"\n fos: ", hedr.fos,
		"\n fis: ", hedr.fis,
		"\n fosl: ", hedr.fosl,
		"\n fisl: ", hedr.fisl,
		"\n file_flags: ", hedr.file_flags,
		"\n sizeof: ", sizeof(hedr),
		"\n--");
}

void bsa_produce_fofi(bsa_t &bsa)
{
	return;
	int l = 0;
	int n = 0;
	for (; n < hedr.fis; n++)
	{
		string s(bsa.finames + l);
		l += s.length() + 1;
		log_("file_t ", s);
		bsa.files.push_back(file_t{s});
	}
}