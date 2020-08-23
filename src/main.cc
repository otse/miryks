#include "dark2.h"

#include "files"

extern "C" {
#include "c/c.h"
#include "c/bsa.h"
#include "c/nif.h"
}

#include "cpp/opengl/types"
#include "cpp/opengl/scene"
#include "cpp/opengl/group"
#include "cpp/opengl/geometry"
#include "cpp/opengl/material"

#include "cpp/mesh.h"

namespace dark2
{
	string OLDRIM_PATH;

	bsa_t interface;
	bsa_t meshes;
	bsa_t animations;

	Camera *camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
} // namespace dark2

#define PATH_TXT "path to oldrim.txt"

using namespace dark2;

void loadBSA()
{
	log_("load bsas");

	//bsa_test();

	//interface = bsa_load(OLDRIM_PATH + "Data/Skyrim - Interface.bsa");
	//animations = bsa_load(OLDRIM_PATH + "Data/Skyrim - Animations.bsa");
	meshes = bsa_load((OLDRIM_PATH + "Data/Skyrim - Meshes.bsa").c_str());

	log_("got bsas");
}

void loadBucket() {
	Mesh *mesh = new Mesh;
	rc_t *rc = bsa_find(&meshes, "meshes\\clutter\\bucket02a.nif");
	cassert_(rc, "mh no bucket02a");
	bsa_read(&meshes, rc);
	nif_t *bucket = nif_alloc();
	bucket->path = rc->path;
	bucket->buf = rc->inf;
	nif_read(bucket);
	mesh->Construct(bucket);
	scene->Add(mesh->base);
}

int main()
{
	log_("dark2 loading");
	OLDRIM_PATH = fread(PATH_TXT);
	cassert_(
		OLDRIM_PATH != "no", "missing" PATH_TXT);
	loadBSA();
	//system("PAUSE");
	program_go();
	oglGo();
	for (int i = 0; i < 10; i++)
	{
	Group *cube = new Group;
	vec3 pos = vec3(
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX,
		(float)rand() / RAND_MAX);
	pos *= 20;
	cube->matrix = glm::translate(mat4(1.0f), pos);
	cube->geometry = new Geometry;
	cube->geometry->material = new Material;
	cube->geometry->SetupMesh();
	cube->Update();
	scene->Add(cube);
	}

	loadBucket();

	nif_test(&meshes);

	program_loop();
	return 1;
}