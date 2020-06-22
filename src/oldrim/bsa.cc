#include "bsa"

#include "dark2.h"

#include "files.hpp"

#define BSA "BSA - "
#define VER 104

#define _hedr bsa.hedr

bsa_t bsa_load(const string &a)
{
	bsa_t bsa;
	bsa.is = ifstream(a, ifstream::binary);
	assert_(
		bsa.is, BSA "cant open");
	bsa.is.read(
		(char *)&_hedr, sizeof(bsa_hedr_t));
	assert_(
		strcmp(
			"BSA\x00", &_hedr.id[0]) == 0,
		BSA "not a bsa");
	assert_(
		_hedr.ver == VER, BSA "not 104");
	bsa_print_hedr(bsa);
	bsa_load_folders(bsa);
	return bsa;
}

void bsa_load_folders(bsa_t &bsa)
{
	bsa.forcds = new forcd_t[_hedr.fos];
	bsa.is.read(
		(char *)bsa.forcds, _hedr.fos * sizeof(forcd_t));
	int n = 0;
	for (; n < _hedr.fos; n++)
		bsa_print_forcd(bsa, n);
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
		"\n ver: ", _hedr.ver,
		"\n offset: ", _hedr.offset,
		"\n flags: ", _hedr.flags,
		"\n fos: ", _hedr.fos,
		"\n fis: ", _hedr.fis,
		"\n fosl: ", _hedr.fosl,
		"\n fisl: ", _hedr.fisl,
		"\n file_flags: ", _hedr.file_flags,
		"\n sizeof: ", sizeof(_hedr),
		"\n--");
}