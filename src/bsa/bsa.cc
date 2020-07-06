#include "dark2.h"
#include "bsa"

#include "files"

#define BSA "bsa - "
#define VER 104
#define VER_SE 105

api bsa_t bsa_load(const string &s)
{
#define hedr bsa.hdr
	bsa_t bsa;
	bsa.is = ifstream(s, ifstream::binary);
	assert_(
		bsa.is, BSA "cant open? try path to oldrim.txt");
	bsa.is.read((char *)&hedr, sizeof(hedr_t));
	assert_(
		strcmp(
			"BSA\x00", (char *)&hedr.id) == 0,
		BSA "not a bsa");
	assert_(
		hedr.ver != VER_SE, BSA "ver 105 is SE");
	assert_(
		hedr.ver == VER, BSA "not 104");
	bsa_read_folder_records(bsa);
	bsa_read_file_records(bsa);
	bsa_read_filenames(bsa);
	bsa_resources(bsa);
	return bsa;
}

void bsa_read_folder_records(bsa_t &b)
{
#define hedr b.hdr
	b.fld = new fld_t[hedr.folders];
	b.is.read((char *)b.fld, hedr.folders * sizeof(fld_t));
}

void bsa_read_file_records(bsa_t &b)
{
#define hedr b.hdr
	b.fle = new fle_t *[hedr.folders];
	b.ca = new const char *[hedr.folders];
	for (int i = 0; i < hedr.folders; i++)
	{
	const int num = b.fld[i].num;
	b.fle[i] = new fle_t [num];
	b.ca[i] = bsa_read_bzstring(b);
	b.is.read((char *)b.fle[i], sizeof(fle_t) * num);
	}
}

void bsa_read_filenames(bsa_t &b)
{
#define hedr b.hdr
	char *buf = new char[hedr.filesl];
	b.cb = new const char *[hedr.files];
	b.is.read(buf, hedr.filesl);
	int i = 0, j = 0, n = 0;
	while (i++ < hedr.filesl)
	{
	if (buf[i] != '\0')
		continue;
	b.cb[n++] = &buf[j];
	j = i + 1;
	}
}

char *bsa_read_bzstring(bsa_t &b)
{
	int c = 0;
	b.is.read((char *)&c, 1);
	char *name = new char [c];
	b.is.read((char *)name, c);
	return name;
}

char *bsa_path(bsa_t &b, int i, int r)
{
#define HACK "\\"
	char *path = (char *)malloc(strlen(b.ca[i]) + strlen(b.cb[r]) + 2);
	strcpy(path, b.ca[i]);
	strcat(path, HACK);
	strcat(path, b.cb[r]);
	return path;
}

void bsa_resources(bsa_t &b)
{
#define hedr b.hdr
	b.rc = new rc_t *[hedr.files];
	b.r = new int [hedr.folders];
	int r = 0;
	for (int i = 0; i < hedr.folders; i++)
	{
	b.r[i] = r;
	for (int j = 0; j < b.fld[i].num; j++)
	{
	b.rc[r] = new rc_t {b, i, j, r, b.cb[r], bsa_path(b, i, j)};
	r++;
	}
	}
}

api rc_t *bsa_find(bsa_t &b, const char *p)
{
#define hedr b.hdr
	char *stem = fstem(p);
	char *name = fname(p);
	if (!stem || !name)
		return nullptr;
	int cmp;
	int r;
	for (int i = 0; i < hedr.folders; i++)
	{
	cmp = strcmp(stem, b.ca[i]);
	if (cmp)
		continue;
	r = b.r[i];
	for (int j = 0; j < b.fld[i].num; j++)
	{
	cmp = strcmp(name, b.cb[r]);
	if (!cmp)
		return b.rc[r];
	r++;
	}
	}
	free(stem);
	free(name);
	return nullptr;
}

api void bsa_read(bsa_t &b, rc_t *rc) {
	if (!rc)
		return;
	fle_t &fe = b.fle[rc->i][rc->j];
	log_("fe.size ", fe.size);
	rc->buf = (const unsigned char *)malloc(fe.size);
	b.is.seekg(fe.offset, ios_base::beg);
	b.is.read((char *)rc->buf, fe.size);
}
