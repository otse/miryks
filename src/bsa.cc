#include "dark2.h"

#include "bsa.h"

#include "files.h"

#define BSA "BSA - "
#define VER 104

bsa_t bsa_load(const string &a)
{
	bsa_t bsa;
	ifstream is(a, ifstream::binary);
	assert_(
		is, BSA "cant open");
	is.read(
		(char *)&bsa.header, sizeof(bsa_header_t));
	assert_(
		strcmp(
			"BSA\x00", &bsa.header.id[0]) == 0,
		BSA "not a bsa");
	assert_(
		bsa.header.ver == VER, BSA "not 104");
	return bsa;
}

void bsa_print(bsa_t &bsa)
{
	log_(
		"--bsa header--",
		"\n ver: ", bsa.header.ver,
		"\n offset: ", bsa.header.offset,
		"\n flags: ", bsa.header.flags,
		"\n folders: ", bsa.header.folders,
		"\n files: ", bsa.header.files,
		"\n--");
}