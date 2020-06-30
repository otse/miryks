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
	bsa_read_folder_records(bsa);
	bsa_read_file_records(bsa);
	bsa_read_filenames(bsa);
	bsa_print(bsa);
	return bsa;
}

void bsa_read_folder_records(bsa_t &b)
{
#define hedr b.hdr
	b.rcdsf = new rcdf_t[hedr.fos];
	b.is.read(
		(char *)b.rcdsf, hedr.fos * sizeof(rcdf_t));
}

void bsa_read_file_records(bsa_t &b)
{
#define hedr b.hdr
	b.rcdsff = new rcdff_t *[hedr.fos];
	b.charsf = new const char *[hedr.fos];
	for (int i = 0; i < hedr.fos; i++)
	{
		const int num = b.rcdsf[i].num;
		b.rcdsff[i] = new rcdff_t[num];
		b.charsf[i] = bsa_read_bzstring(b);
		b.is.read(
			(char *)b.rcdsff[i], sizeof(rcdff_t) * num);
	}
}

void bsa_read_filenames(bsa_t &b)
{
#define hedr b.hdr
	char *buf = new char[hedr.fil];
	b.charsff = new const char *[hedr.fis];
	b.is.read(buf, hedr.fil);
	int i = 0, j = 0, n = 0;
	while (i++ < hedr.fil)
	{
		if (buf[i] != '\0')
			continue;
		b.charsff[n++] = &buf[j];
		j = i + 1;
	}
}

const char *bsa_read_bzstring(bsa_t &b)
{
	int c = 0;
	b.is.read((char *)&c, 1);
	const char *name = new char[c];
	b.is.read((char *)name, c);
	return name;
}

// -----------------------------------------------