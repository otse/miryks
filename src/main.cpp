#include <dark2/dark2.h>

extern "C"
{
#include "putc.h"
#include "bsa.h"
#include <nifp/nifp.h>
#include "esp.h"
}

#include <opengl/types.h>
#include <opengl/camera.h>
#include <opengl/scene.h>
#include <opengl/group.h>
#include <opengl/renderable.h>
#include <opengl/geometry.h>
#include <opengl/material.h>

#include <dark2/files.h>

#include <dark2/level.h>
#include <dark2/mesh.h>

namespace dark2
{
	namespace viewer
	{
		Mesh *mesh = nullptr;
		Renderable *object = nullptr;
	} // namespace viewer

	unsigned int fps = 0;

	Level *dungeon;

	std::string pathToOldrim;

	FirstPersonCamera *first_person_camera;
	ViewerCamera *viewer_camera;
	RenderTarget *render_target;

	int width = 2560;
	int height = 1440;
	float delta = 1;
} // namespace dark2

namespace dark2
{
	const char *dataFolder = "Data/";

	nifp *loadNif(rc *rc)
	{
		cassert(rc, "mh no rc");
		bsa_read(rc);
		nifp *nif = malloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nifp_read(nif);
		// nifp_save(rc, nif);
		return nif;
	}

	esp *loadPlugin(const char *filename)
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

	bsa *loadArchive(const char *filename)
	{
		std::string path = pathToOldrim + dataFolder + filename;
		if (exists_test3(path))
			return bsa_load(path.c_str());
		else
			return bsa_load(filename);
		return nullptr;
	}

	void viewer::view(rc *rc)
	{
		if (mesh)
		{
			scene->Remove(object);
			delete mesh;
			delete object;
		}
		nifp *nif = nifp_saved(rc);
		if (nif == NULL)
		{
			nif = loadNif(rc);
			nifp_save(rc, nif);
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
	printf("loading\n");
	cassert(exists("path to oldrim.txt"), "missing `path to oldrim.txt`");
	pathToOldrim = fread("path to oldrim.txt");
	get_plugins()[0] = loadPlugin("Skyrim.esm");
	printf("loaded skyrim\n");
	get_plugins()[1] = loadPlugin("Padstow.esp");
	get_archives()[0] = loadArchive("Skyrim - Meshes.bsa");
	get_archives()[1] = loadArchive("Skyrim - Textures.bsa");
	get_archives()[2] = loadArchive("HighResTexturePack01.bsa");
	get_archives()[3] = loadArchive("HighResTexturePack02.bsa");
	get_archives()[4] = loadArchive("HighResTexturePack03.bsa");
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	OGLGo();
	camera = first_person_camera;
#if 0
	rc *rc = bsa_find_more("meshes\\clutter\\bucket02a.nif", 0x1);
	viewer::view(rc);
#endif
	//nif_test();
	//nifp_test();
	dungeon = new Level("PadstowDungeon"); // <-- interior to load
	programLoop();
	return 1;
}
