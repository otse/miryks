#include "dark2.h"

#include "files.h"

extern "C"
{
#include "putc.h"
#include "bsa.h"
#include "nif.h"
#include "esp.h"
}

#include "opengl/types.h"
#include "opengl/camera.h"
#include "opengl/scene.h"
#include "opengl/group.h"
#include "opengl/renderable.h"
#include "opengl/geometry.h"
#include "opengl/material.h"

#include "level.h"
#include "mesh.h"

namespace dark2
{
	namespace viewer
	{
		Mesh *mesh = nullptr;
		Renderable *object = nullptr;
	} // namespace viewer

	Level *dungeon;

	std::string pathToOldrim;

	esp *skyrim;
	esp *padstow;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *viewer_camera;

	int width = 1920;
	int height = 1080;
	float delta = 1;
} // namespace dark2

namespace dark2
{
	const char *dataFolder = "Data/";

	nif *LoadNif(rc *rc)
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

	esp *LoadPlugin(const char *filename)
	{
		std::string path = pathToOldrim + dataFolder + filename;
		char *buf;
		int ret;
		// Try load plugin from skyrim else local folder
		(ret = fbuf(path.c_str(), &buf)) == -1 ? (ret = fbuf(filename, &buf)) : void();
		if (ret == -1)
			return nullptr;
		esp *plugin = plugin_slate();
		plugin->name = filename;
		plugin->buf = buf;
		plugin->filesize = ret;
		plugin_load(plugin);
		return plugin;
	}

	bsa *LoadArchive(const char *filename)
	{
		std::string path = pathToOldrim + dataFolder + filename;
		if (exists_test3(path))
			return bsa_load(path.c_str());
		else
			return bsa_load(filename);
		return nullptr;
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
			nif = LoadNif(rc);
			nif_save(rc, nif);
		}
		mesh = new Mesh;
		mesh->Construct(nif);
		mat4 mat = translate(mat4(1.0), first_person_camera->pos);
		object = new Renderable(mat, mesh->baseGroup);
		scene->Add(object);
		camera = viewer_camera;
		HideCursor();
		viewer_camera->disabled = false;
		viewer_camera->pos = object->aabb.center();
		viewer_camera->radius = object->aabb.radius2() * 2;
	}
} // namespace dark2

int main()
{
	using namespace dark2;
	log_("dark2 loading");
	cassert(exists("path to oldrim.txt"), "missing path to oldrim.txt");
	pathToOldrim = fread("path to oldrim.txt");
	get_plugins()[0] = LoadPlugin("Skyrim.esm");
	get_plugins()[1] = LoadPlugin("Padstow.esp");
	get_archives()[0] = LoadArchive("Skyrim - Meshes.bsa");
	get_archives()[1] = LoadArchive("Skyrim - Textures.bsa");
	get_archives()[2] = LoadArchive("HighResTexturePack01.bsa");
	get_archives()[3] = LoadArchive("HighResTexturePack02.bsa");
	get_archives()[4] = LoadArchive("HighResTexturePack03.bsa");
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	OGLGo();
	camera = first_person_camera;
	rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	viewer::spotlight(rc);
	nif_test();
	dungeon = new Level("PadstowDungeon"); // <-- interior to load
	programLoop();
	return 1;
}
