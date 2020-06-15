#include "dark2.h"

#include "files.h"

#define BSA "BSA - "
#define VER 104

struct header_t
{
	char file_id[4];
	unsigned long ver, offset, flags, folders, files, folder_max, file_max, file_flags;
};

struct bsa_t
{
	vector<unsigned char> data;
	header_t header;
};

bsa_t bsa_load(const string &a)
{
	bsa_t bsa;

	MAGIC(BSA "load ", a);

	if (fstat(a) == false)
		EXIT(BSA "archive not found");

	bsa.data = freadbin(a);

	if (strcmp("BSA\x00", (char *)&bsa.data[0]) != 0)
		EXIT(BSA "wrong id");

	memcpy(&bsa.header, &bsa.data[0], sizeof(header_t));
	if (bsa.header.ver != VER)
		EXIT(BSA "wrong ver");

	MAGIC(BSA "done");

	return bsa;
}

void bsa_print(bsa_t &bsa)
{
	MAGIC(
		"--bsa header--",
		"\n ver: ", bsa.header.ver,
		"\n offset: ", bsa.header.offset,
		"\n flags: ", bsa.header.flags,
		"\n folders: ", bsa.header.folders,
		"\n files: ", bsa.header.files,
		"\n--");
}