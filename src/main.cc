// xx

#include "dark2.h"
#include "0x68.h"

//#include <stdio.h>
#include <iostream>

#include "files.h"

int main()
{

    std::cout << "dark2 loading" << std::endl;

    _string_ skyrim_path = "F:/Steam/steamapps/common/Skyrim/";

    bsa_load(skyrim_path + "Data/Skyrim - Interface.bsa");

    //system("PAUSE");

    return 1;
}