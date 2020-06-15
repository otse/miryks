// xx

#include "dark2.h"

#include "bsa.h"

//#include <stdio.h>
#include <iostream>

#include "files.h"

namespace dark2
{
    string path_to_skyrim;
}

using namespace dark2;

int main()
{
    MAGIC("dark2 loading");

    path_to_skyrim = fread("path to skyrim.txt");

    if (path_to_skyrim == "no")
        EXIT("where is `path to skyrim.txt`");

    MAGIC("path to skyrim.txt: ", path_to_skyrim);

    bsa_t bsa_interface = bsa_archive_load("Skyrim - Interface");

    bsa_print_info(bsa_interface);

    //system("PAUSE");

    return 1;
}