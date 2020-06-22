#include "bsa"

#include "dark2.h"

#include "files.hpp"

#define BSA "BSA - "
#define VER 104

bsa_t bsa_load(const string &a)
{
	bsa_t bsa;
	bsa.is = ifstream(a, ifstream::binary);
	assert_(
		bsa.is, BSA "cant open");
	bsa.is.read(
		(char *)&bsa.hedr, sizeof(bsa_hedr_t));
	assert_(
		strcmp(
			"BSA\x00", &bsa.hedr.id[0]) == 0,
		BSA "not a bsa");
	assert_(
		bsa.hedr.ver == VER, BSA "not 104");
	bsa_print_hedr(bsa);
	bsa_load_folder_rcd(bsa);
	return bsa;
}

void bsa_load_folder_rcd(bsa_t &bsa)
{
	bsa.forcds = new forcd_t[bsa.hedr.nfo];
	bsa.is.read(
		(char *)bsa.forcds, bsa.hedr.nfo * sizeof(forcd_t));
	int n = 0;
	for (; n < bsa.hedr.nfo; n++)
		bsa_print_folder_rcd(bsa, n);
}

void bsa_print_folder_rcd(bsa_t &bsa, int n)
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
		"\n ver: ", bsa.hedr.ver,
		"\n offset: ", bsa.hedr.offset,
		"\n flags: ", bsa.hedr.flags,
		"\n nfo: ", bsa.hedr.nfo,
		"\n nfi: ", bsa.hedr.nfi,
		"\n lenfo: ", bsa.hedr.lenfo,
		"\n lenfi: ", bsa.hedr.lenfi,
		"\n file_flags: ", bsa.hedr.file_flags,
		"\n sizeof: ", sizeof(bsa.hedr),
		"\n--");
}