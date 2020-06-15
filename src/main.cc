// xx

#include "dark2.h"

#include "bsa.h"

//#include <stdio.h>
#include <iostream>

#include "files.h"

#define DATA "Data/"

namespace dark2
{
    string SKYRIMPATH;
}

using namespace dark2;

int main()
{
    MAGIC("dark2 loading");

    SKYRIMPATH = fread("path to skyrim.txt");

    if (SKYRIMPATH == "no")
        EXIT("");

    bsa_t interface = bsa_load(SKYRIMPATH + DATA + "Skyrim - Interface.bsa");
    bsa_print(interface);

    //system("PAUSE");

    return 1;
}