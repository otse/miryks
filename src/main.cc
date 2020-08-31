#include "dark2.h"

#include "files"

extern "C"
{
#include "c/c.h"
#include "c/bsa.h"
#include "c/nif.h"
}

#include "cpp/opengl/types"
#include "cpp/opengl/camera"
#include "cpp/opengl/scene"
#include "cpp/opengl/group"
#include "cpp/opengl/renderable"
#include "cpp/opengl/geometry"
#include "cpp/opengl/material"

#include "cpp/mesh.h"

namespace dark2
{
	namespace viewer
	{
	Mesh *mesh = nullptr;
	Renderable *object = nullptr;
	}

	std::string OLDRIM;

	bsa_t *interface;
	bsa_t *meshes;
	bsa_t *animations;
	bsa_t *textures;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *viewer_camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
} // namespace dark2

#define PATH_TXT "path to oldrim.txt"

nif_t *dark2::make_nif(rc_t *rc)
{
	cassert_(rc, "mh no rc");
	bsa_read(rc);
	nif_t *nif = nif_alloc();
	nif->path = rc->path;
	nif->buf = rc->buf;
	nif_read(nif);
	nif_save(rc, nif);
	return nif;
}

void dark2::viewer::spotlight(rc_t *rc)
{
	if (mesh)
	{
		scene->Remove(object);
		delete mesh;
		delete object;
	}
	nif_t *nif = nif_saved(rc);
	if (nif == NULL)
		nif = make_nif(rc);
	mesh = new Mesh;
	mesh->Construct(nif);
	object = new Renderable(mat4(1.0), mesh->baseGroup);
	scene->Add(object);
	camera = viewer_camera;
	viewer_camera->pos = object->aabb.center();
	viewer_camera->radius = object->aabb.radius2() * 2;
}

int main()
{
	using namespace dark2;
	log_("dark2 loading");
	cassert_(exists(PATH_TXT), "missing " PATH_TXT);
	OLDRIM = fread(PATH_TXT);
	
	meshes = bsa_load((OLDRIM + "Data/Skyrim - Meshes.bsa").c_str());
	textures = bsa_load((OLDRIM + "Data/Skyrim - Textures.bsa").c_str());
	bsa_t *array[2] = {meshes, textures};
	bsas_add_to_loaded(&bsas, array, 2);
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	oglGo();
	camera = first_person_camera;
	rc_t *rc = bsa_find(dark2::meshes, "meshes\\clutter\\bucket02a.nif");
	viewer::spotlight(rc);
	nif_test(meshes);
	programLoop();
	return 1;
}