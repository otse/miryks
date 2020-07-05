#include "bsa"

#include "dark2.h"

using namespace dark2;


void bsa_test() {

	bsa_t bsa = bsa_load(OLDRIM_PATH + "Data/Skyrim - Meshes.bsa");

	bsa_print(bsa);

}