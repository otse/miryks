#include <vector>

struct bsa_header_t
{
	char id[4];
	unsigned long ver, offset, flags, folders, files, folder_max, file_max, file_flags;
};

struct bsa_t
{
	vector<unsigned char> data;
	bsa_header_t header;
};

bsa_t bsa_load(const string &a);

void bsa_print(bsa_t &bsa);