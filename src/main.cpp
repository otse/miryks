#include <gloom/dark2.h>

#include <algorithm>

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
#include <gloom/actor.h>

namespace gloom
{
	namespace viewer
	{
		Mesh *mesh = nullptr;
		DrawGroup *drawGroup = nullptr;
	} // namespace viewer

	unsigned int fps = 0;

	Level *dungeon = nullptr;
	Actor *someDraugr = nullptr;
	Actor *meanSkelly = nullptr;
	Human *someHuman = nullptr;
	Player *player1 = nullptr;

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

	Rc *loadrc(const char *prepend = "meshes\\", const char *path = "", unsigned long flags = 0x1)
	{
		std::string str = prepend;
		str += path;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		Rc *rc = bsa_find_more(str.c_str(), flags);
		bsa_read(rc);
		return rc;
	}

	Nifp *loadnifp(Rc *rc, int save)
	{
		cassert(rc, "mh no rc");
		bsa_read(rc);
		Nifp *nif = malloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nifp_read(nif);
		if (save)
			nifp_save(rc, nif);
		return nif;
	}

	Esp *LoadPlugin(const char *filename)
	{
		std::string path = pathToOldrim + dataFolder + filename;
		char *buf;
		int ret;
		// Try load plugin from skyrim else local folder
		(ret = fbuf(path.c_str(), &buf)) == -1 ? (ret = fbuf(filename, &buf)) : void();
		if (ret == -1)
			return nullptr;
		Esp *plugin = plugin_slate();
		plugin->name = filename;
		plugin->buf = buf;
		plugin->filesize = ret;
		plugin_load(plugin);
		return plugin;
	}

	Bsa *LoadArchive(const char *filename)
	{
		std::string path = pathToOldrim + dataFolder + filename;
		printf("%s\n", path.c_str());
		if (exists(path.c_str()))
			return bsa_load(path.c_str());
		else
			return bsa_load(filename);
		return nullptr;
	}

	void viewer::view(Rc *rc)
	{
		if (mesh)
		{
			scene->Remove(drawGroup);
			delete mesh;
			delete drawGroup;
		}
		Nifp *nif = nifp_saved(rc);
		if (nif == NULL)
		{
			nif = loadnifp(rc, 1);
			nifp_save(rc, nif);
		}
		mesh = new Mesh;
		mesh->Construct(nif);
		drawGroup = new DrawGroup(mesh->baseGroup, glm::translate(mat4(1.0), first_person_camera->pos));
		scene->Add(drawGroup);
		HideCursor();
		camera = viewer_camera;
		viewer_camera->disabled = false;
		viewer_camera->pos = drawGroup->aabb.center();
		//viewer_camera->pos = first_person_camera->pos;
		viewer_camera->radius = drawGroup->aabb.radius2() * 2;
	}
} // namespace gloom

int main()
{
	using namespace gloom;
	printf("loading\n");
	pathToOldrim = fread("path to oldrim.txt");
	cassert(exists("path to oldrim.txt"), "missing path to oldrim.txt");
	cassert(exists((pathToOldrim + "TESV.exe").c_str()), "cant find tesv.exe, check your path");
	get_plugins()[0] = LoadPlugin("Skyrim.esm");
	if (get_plugins()[0])
		printf("loaded skyrim.esm!\n");
	get_plugins()[1] = LoadPlugin("Gloom.esp");
	get_archives()[0] = LoadArchive("Skyrim - Meshes.bsa");
	get_archives()[1] = LoadArchive("Skyrim - Textures.bsa");
	get_archives()[2] = LoadArchive("Skyrim - Animations.bsa");
	//get_archives()[3] = LoadArchive("HighResTexturePack01.bsa");
	//get_archives()[4] = LoadArchive("HighResTexturePack02.bsa");
	//get_archives()[5] = LoadArchive("HighResTexturePack03.bsa");
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
	nifp_test();
	dungeon = new Level("GloomGen"); // <-- interior to load
	//someDraugr = new Actor("DraugrRace", "meshes\\actors\\draugr\\character assets\\draugrmale.nif");
	//someDraugr->PutDown("gloomgendraugr");
	//meanSkelly = new Actor("DraugrRace", "meshes\\actors\\draugr\\character assets\\draugrskeleton.nif");
	//meanSkelly->PutDown("gloomgenskeleton");
	//someHuman = new Human();
	//someHuman->Place("gloomgenman");

	//player1 = new Player();

	programLoop();
	return 1;
}
