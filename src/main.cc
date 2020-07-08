// xx

#include "dark2.h"

#include "files"

extern "C" {
#include "bsa/bsa.h"
}
#include "nif/nif.h"

#include "oldrim_interface.hpp"

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

	Camera_t *camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
} // namespace dark2

#define PATH_TXT "path to oldrim.txt"

using namespace dark2;

void load_bsas()
{
	//bsa_test();

	//interface = bsa_load(OLDRIM_PATH + "Data/Skyrim - Interface.bsa");
	//animations = bsa_load(OLDRIM_PATH + "Data/Skyrim - Animations.bsa");
	meshes = bsa_load((OLDRIM_PATH + "Data/Skyrim - Meshes.bsa").c_str());
}

int main()
{
	log_("dark2 loading");
	OLDRIM_PATH = fread(PATH_TXT);
	assert_(
		OLDRIM_PATH != "no", "missing" PATH_TXT);
	load_bsas();
	//system("PAUSE");
	program_go();
	oglGo();
	for (int i = 0; i < 100; i++)
	{
	Group_t *cube = new Group_t;
	vec3 pos = vec3(
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX);
	pos *= 20;
	cube->matrix = glm::translate(mat4(1.0f), pos);
	cube->geometry = new Geometry_t;
	cube->geometry->material = new Material_t;
	cube->geometry->SetupMesh();
	cube->Update();
	scene->Add(cube);
	}
	/*rc_t *rc = bsa_find(meshes, "meshes\\clutter\\bucket02a.nif");
	assert_(rc, "cant find bucket");
	nif_t *bucket = new nif_t;
	bucket->path = rc->path;
	bucket->buf = rc->buf;
	bsa_read(meshes, rc);
	nif_make(rc, bucket);*/
	program_loop();
	return 1;
}