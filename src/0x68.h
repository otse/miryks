#include "idiom.h"
#include "files.h"

void bsa_load(string2_t t) {
    tse_log("bsa_load ", t);

    if (!fexists(t))
        tse_exit("boo");

    auto data = fread2(t);
    
    //return 1;
}