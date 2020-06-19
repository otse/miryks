// xx

#include "dark2.h"

#include "files.hpp"

#include "oldrim/bsa"

#include "opengl/types"
#include "opengl/scene"
#include "opengl/group"
#include "opengl/geometry"
#include "opengl/material"

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
} // namespace dark2

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
	program_go();
	opengl_go();
	for (int i = 0; i < 100; i++)
	{
		group_t *cube = new group_t;
		vec3 pos = vec3(
			(float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX) * 20.f;
		cube->matrix = glm::translate(mat4(1.0f), pos);
		cube->geometry = new geometry_t;
		cube->geometry->material = new material_t;
		cube->geometry->SetupMesh();
		cube->Update();
		scene->Add(cube);
	}
	program_loop();
	return 1;
}