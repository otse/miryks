#include "arc"

#include "dark2.h"

#include "files.hpp"


arc_t arc_load(const string &s)
{
    arc_t a;
    a.bsa = bsa_load(s);
    return a;
}

void arc_init(arc_t &a) {
    
}