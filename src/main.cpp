#include "dark2.h"

#include "files"

extern "C"
{
#include "putc.h"
#include "bsa.h"
#include "nif.h"
#include "esp.h"
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

#define PATH_TXT "path to oldrim.txt"

namespace dark2
{
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
	Bsa *hirestexturepack01;
	Bsa *hirestexturepack02;
	Bsa *hirestexturepack03;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *viewer_camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
} // namespace dark2

namespace dark2
{
	nif *GetNif(rc *rc)
	{
		cassert(rc, "mh no rc");
		bsa_read(rc);
		nif *nif = nif_alloc();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nif_read(nif);
		// nif_save(rc, nif);
		return nif;
	}

	esp *GetPlugin(const char *filename)
	{
		esp *plugin = nullptr;
		std::string path = OLDRIM + "Data/" + filename;
		plugin = esp_load(path.c_str(), 1);
		if (!plugin)
			plugin = esp_load(filename, 0);
		return plugin;
	}

	void viewer::spotlight(rc *rc)
	{
		if (mesh)
		{
			scene->Remove(object);
			delete mesh;
			delete object;
		}
		nif *nif = nif_saved(rc);
		if (nif == NULL)
		{
			nif = GetNif(rc);
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
} // namespace dark2

int main()
{
	using namespace dark2;
	log_("dark2 loading");
	cassert(exists(PATH_TXT), "missing " PATH_TXT);
	OLDRIM = fread(PATH_TXT);
	skyrim = GetPlugin("Skyrim.esm");
	testMod = GetPlugin("TestMod.esp");
	get_plugins()[0] = skyrim;
	get_plugins()[1] = testMod;
	meshes = bsa_load((OLDRIM + "Data/Skyrim - Meshes.bsa").c_str());
	textures = bsa_load((OLDRIM + "Data/Skyrim - Textures.bsa").c_str());
	if (exists_test3(OLDRIM + "Data/HighResTexturePack01.bsa"))
	{
		hirestexturepack01 = bsa_load((OLDRIM + "Data/HighResTexturePack01.bsa").c_str());
		hirestexturepack02 = bsa_load((OLDRIM + "Data/HighResTexturePack02.bsa").c_str());
		hirestexturepack03 = bsa_load((OLDRIM + "Data/HighResTexturePack03.bsa").c_str());
	}
	textures = bsa_load((OLDRIM + "Data/Skyrim - Textures.bsa").c_str());
	bsa *array[5] = {meshes, textures, hirestexturepack01, hirestexturepack02, hirestexturepack03};
	bsa_set_loaded(array, 5);
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	oglGo();
	camera = first_person_camera;
	rc *rc = bsa_find(dark2::meshes, "meshes\\clutter\\bucket02a.nif");
	viewer::spotlight(rc);
	nif_test(meshes);
	dungeon = new Level("Dark2Schmuck");
	programLoop();
	return 1;
}
