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
	bool good = false;
	string name;
	vector<unsigned char> data;
	header_t header;
};

bsa_t bsa_archive_load(string2_t a)
{
	bsa_t bsa;
	bsa.name = a;

	MAGIC(BSA "archive load ", bsa.name);

	const string file = dark2::path_to_skyrim + "Data/" + bsa.name + ".bsa";

	if (fstat(file) == false)
		EXIT(BSA "archive not found");

	bsa.data = freadbin(file);

	if (strcmp("BSA\x00", (char *)&bsa.data[0]) != 0)
		EXIT(BSA "wrong id digits");

	memcpy(&bsa.header, &bsa.data[0], sizeof(header_t));
	if (bsa.header.ver != VER)
		EXIT(BSA "wrong version");

	MAGIC(BSA "read");

	bsa.good = true;

	return bsa;
}

void bsa_print_info(bsa_t &bsa)
{
	MAGIC(
		BSA"print info",
		"\n name: ", bsa.name,
		"\n good: ", bsa.good,
		"\n ver: ", bsa.header.ver,
		"\n offset: ", bsa.header.offset,
		"\n flags: ", bsa.header.flags,
		"\n folders: ", bsa.header.folders,
		"\n files: ", bsa.header.files);
}