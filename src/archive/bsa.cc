#include "bsa"

#include "dark2.h"

#include "files.hpp"

#define BSA "BSA - "
#define VER 104
#define VER_SE 105

api bsa_t bsa_load(const string &s) {
#define hedr bsa.hdr
	bsa_t bsa;
	bsa.is = ifstream(s, ifstream::binary);
	assert_(
		bsa.is, BSA "cant open");
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
	bsa_sort(bsa);
	return bsa;
}

void bsa_read_folder_records(bsa_t &b) {
#define hedr b.hdr
	b.aa = new fld_t [hedr.folders];
	b.is.read(
		(char *)b.aa, hedr.folders * sizeof(fld_t));
}

void bsa_read_file_records(bsa_t &b) {
#define hedr b.hdr
	b.bb = new fle_t *[hedr.folders];
	b.ca = new const char *[hedr.folders];//0
	for (int i = 0; i < hedr.folders; i++)
	{
	const int num = b.aa[i].num;
	b.bb[i] = new fle_t [num];
	b.ca[i] = bsa_read_bzstring(b);
	b.is.read((char *)b.bb[i], sizeof(fle_t) * num);
	}
}

void bsa_read_filenames(bsa_t &b) {
#define hedr b.hdr
	char *buf = new char [hedr.filesl];
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

char *bsa_read_bzstring(bsa_t &b) {
	int c = 0;
	b.is.read((char *)&c, 1);
	char *name = new char [c];
	b.is.read((char *)name, c);
	return name;
}

char *bsa_path(bsa_t &b, int i, int r) {
	char *path = (char*)malloc(strlen(b.ca[i]) + strlen(b.cb[r]) + 2);
	strcpy(path, b.ca[i]);
    strcat(path, "\\");
    strcat(path, b.cb[r]);
	return path;
}

void bsa_resources(bsa_t &b) {
#define hedr b.hdr
	int r = 0;
	b.rc = new rc_t *[hedr.files];
	int i = 0;
	for (; i < hedr.folders; i++)
	{
	fld_t *fld = &b.aa[i];
	int j = 0;
	for (; j < fld->num; j++)
	{
	fle_t *fle = &b.bb[i][j];
	char *path = bsa_path(b, i, r);
	//log_("bsa_path ", path);
	rc_t *rc = new rc_t {r, fld, fle, b.cb[r], path};
	b.rc[r] = rc;
	r++;
	}
	}
}

void bsa_sort(bsa_t &b) {

}

api bool bsa_find(bsa_t &b, const char *path) {
#define hedr b.hdr
	int i = 0;
	for (; i < hedr.files; i++)
	{
	rc_t *rc = b.rc[i];
	int cmp = strcmp(path, rc->path);
	if (!cmp)
		return true;
	}
	return false;
}