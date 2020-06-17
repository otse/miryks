// xx

#include "dark2.h"

#include "bsa.h"

//#include <stdio.h>
#include <iostream>

#include "files.h"

namespace dark2
{
	string SKYRIM_PATH;

	bsa_t interface;
    bsa_t meshes;
    bsa_t animations;

	camera_t *camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
}

#define PATH_TXT "path to skyrim.txt"

using namespace dark2;

void load_archives()
{
	interface = bsa_load(SKYRIM_PATH + "Data/Skyrim - Interface.bsa");
	animations = bsa_load(SKYRIM_PATH + "Data/Skyrim - Animations.bsa");
	meshes = bsa_load(SKYRIM_PATH + "Data/Skyrim - Meshes.bsa");
}

int main()
{
	log_("dark2 loading");
	
	SKYRIM_PATH = fread(PATH_TXT);
	assert_(
		SKYRIM_PATH != "no", "missing" PATH_TXT);
	load_archives();
	bsa_print(animations);
	//system("PAUSE");
	
	program_loop();
	return 1;
}