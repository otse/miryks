#include <gloom/dark2.h>

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
#include <opengl/bound.h>
#include <opengl/geometry.h>
#include <opengl/material.h>

#include <gloom/files.h>

#include <gloom/level.h>
#include <gloom/mesh.h>

namespace gloom
{
	namespace viewer
	{
		Mesh *mesh = nullptr;
		DrawGroup *drawGroup = nullptr;
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
} // namespace gloom

namespace gloom
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
			scene->Remove(drawGroup);
			delete mesh;
			delete drawGroup;
		}
		nifp *nif = nifp_saved(rc);
		if (nif == NULL)
		{
			nif = loadNif(rc);
			nifp_save(rc, nif);
		}
		mesh = new Mesh;
		mesh->Construct(nif);
		drawGroup = new DrawGroup(mesh->baseGroup, glm::translate(mat4(1.0), first_person_camera->pos));
		scene->Add(drawGroup);
		camera = viewer_camera;
		HideCursor();
		viewer_camera->disabled = false;
		viewer_camera->pos = drawGroup->group->aabb.center();
		viewer_camera->radius = drawGroup->group->aabb.radius2() * 2;
	}
} // namespace gloom

int main()
{
	using namespace gloom;
	printf("loading\n");
	cassert(exists("path to oldrim.txt"), "missing `path to oldrim.txt`");
	pathToOldrim = fread("path to oldrim.txt");
	get_plugins()[0] = loadPlugin("Skyrim.esm");
	printf("loaded skyrim.esm!\n");
	get_plugins()[1] = loadPlugin("Gloom.esp");
	get_archives()[0] = loadArchive("Skyrim - Meshes.bsa");
	get_archives()[1] = loadArchive("Skyrim - Textures.bsa");
	//get_archives()[2] = loadArchive("HighResTexturePack01.bsa");
	//get_archives()[3] = loadArchive("HighResTexturePack02.bsa");
	//get_archives()[4] = loadArchive("HighResTexturePack03.bsa");
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	openglScene();
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
