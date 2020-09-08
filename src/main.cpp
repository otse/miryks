#include "dark2.h"

#include "files"

extern "C"
{
#include "c.h"
#include "bsa.h"
#include "nif.h"
}

#include "opengl/types"
#include "opengl/camera"
#include "opengl/scene"
#include "opengl/group"
#include "opengl/renderable"
#include "opengl/geometry"
#include "opengl/material"

#include "level.h"
#include "mesh.h"

namespace dark2
{
	vec2 *cast_vec_2(float *f)
	{
		return reinterpret_cast<vec2 *>(f);
	}
	vec3 *cast_vec_3(float *f)
	{
		return reinterpret_cast<vec3 *>(f);
	}
	vec4 *cast_vec_4(float *f)
	{
		return reinterpret_cast<vec4 *>(f);
	}
	mat3 *cast_mat_3(float *f)
	{
		return reinterpret_cast<mat3 *>(f);
	}
	mat4 *cast_mat_4(float *f)
	{
		return reinterpret_cast<mat4 *>(f);
	}
	
	namespace viewer
	{
		Mesh *mesh = nullptr;
		Renderable *object = nullptr;
	} // namespace viewer

	Level *dungeon;

	std::string OLDRIM;

	Esp *skyrim;
	Esp *testMod;
	Bsa *interface;
	Bsa *meshes;
	Bsa *animations;
	Bsa *textures;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *viewer_camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
} // namespace dark2

#define PATH_TXT "path to oldrim.txt"

Nif *dark2::make_nif(Rc *rc)
{
	cassert_(rc, "mh no rc");
	bsa_read(rc);
	Nif *nif = nif_alloc();
	nif->path = rc->path;
	nif->buf = rc->buf;
	nif_read(nif);
	//nif_save(rc, nif);
	return nif;
}

void dark2::viewer::spotlight(Rc *rc)
{
	if (mesh)
	{
		scene->Remove(object);
		delete mesh;
		delete object;
	}
	Nif *nif = nif_saved(rc);
	if (nif == NULL) {
		nif = make_nif(rc);
		nif_save(rc, nif);
	}
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
	skyrim = esp_load((OLDRIM + "Data/Skyrim.esm").c_str());
	testMod = esp_load((OLDRIM + "Data/TestMod.esp").c_str());
	plugins[0] = skyrim;
	plugins[1] = testMod;
	meshes = bsa_load((OLDRIM + "Data/Skyrim - Meshes.bsa").c_str());
	textures = bsa_load((OLDRIM + "Data/Skyrim - Textures.bsa").c_str());
	struct bsa *array[2] = {meshes, textures};
	bsas_add_to_loaded(&bsas, array, 2);
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	oglGo();
	camera = first_person_camera;
	Rc *rc = bsa_find(dark2::meshes, "meshes\\clutter\\bucket02a.nif");
	viewer::spotlight(rc);
	nif_test(meshes);
	dungeon = new Level();
	programLoop();
	return 1;
}
