#include "idiom.h"
#include "files.h"

#define BSA "bsa - "
#define VER 104
#define vol(v, o, l) \
    memcpy(v, &data[o], l);

struct header_t {
    char file_id[4];
    unsigned long version;
    unsigned long offset;
    unsigned long archive_flags;
    unsigned long folder_count;
    unsigned long file_count;
    unsigned long total_folder_name_length;
    unsigned long total_file_name_length;
    unsigned long file_flags;
};

void bsa_load(string2_t t)
{
    MAGIC(BSA"load ", t);

    if (fstat(t) == false)
        EXIT(BSA"arhcive missing");

    auto data = freadbin(t);

    char fileId[4];
    vol(fileId, 0, 4);

    if (strcmp("BSA\x00", fileId) != 0)
        EXIT(BSA"not a bsa");

    header_t header;
    vol(&header, 0, sizeof(header));

    if (header.version != VER)
        EXIT(BSA"not 0x68");
    
    //return 1;
}