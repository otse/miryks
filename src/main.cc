// xx

#include "dark2.h"

#include "files.hpp"

#include "oldrim/bsa"

#include "opengl/types"
#include "opengl/scene"
#include "opengl/group"
#include "opengl/geometry"


namespace dark2
{
	string OLDRIM_PATH;

	bsa_t interface;
    bsa_t meshes;
    bsa_t animations;

	camera_t *camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
}

#define PATH_TXT "path to oldrim.txt"

using namespace dark2;

void load_archives()
{
	interface = bsa_load(OLDRIM_PATH + "Data/Skyrim - Interface.bsa");
	animations = bsa_load(OLDRIM_PATH + "Data/Skyrim - Animations.bsa");
	meshes = bsa_load(OLDRIM_PATH + "Data/Skyrim - Meshes.bsa");
}

int main()
{
	log_("dark2 loading");
	
	OLDRIM_PATH = fread(PATH_TXT);
	assert_(
		OLDRIM_PATH != "no", "missing" PATH_TXT);
	load_archives();
	bsa_print(animations);
	//system("PAUSE");
	opengl_go();
	program_go();
	group_t *cube = new group_t;
	cube->geometry = new geometry_t;
	cube->geometry->SetupMesh();
	scene->Add(cube);
	program_loop();
	return 1;
}