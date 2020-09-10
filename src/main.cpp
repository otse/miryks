#include "dark2.h"

#include "files.h"

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
	Esp *padstow;
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
		std::string path = OLDRIM + "Data/" + filename;
		const char *buf;
		int ret;
		// Try load plugin from skyrim else locally
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
		std::string path = OLDRIM + "Data/" + filename;
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
	skyrim = LoadPlugin("Skyrim.esm");
	padstow = LoadPlugin("Padstow.esp");
	get_plugins()[0] = skyrim;
	get_plugins()[1] = padstow;
	cassert(skyrim, "cant load skyrim.esm");
	cassert(padstow, "cant load padstow.esp");

	meshes = LoadArchive("Skyrim - Meshes.bsa");
	textures = LoadArchive("Skyrim - Textures.bsa");
	hirestexturepack01 = LoadArchive("HighResTexturePack01.bsa");
	hirestexturepack02 = LoadArchive("HighResTexturePack02.bsa");
	hirestexturepack03 = LoadArchive("HighResTexturePack03.bsa");
	
	get_archives()[0] = meshes;
	get_archives()[1] = textures;
	get_archives()[2] = hirestexturepack01;
	get_archives()[3] = hirestexturepack02;
	get_archives()[4] = hirestexturepack03;
	
	programGo();
	first_person_camera = new FirstPersonCamera;
	viewer_camera = new ViewerCamera;
	oglGo();
	camera = first_person_camera;
	rc *rc = bsa_find(dark2::meshes, "meshes\\clutter\\bucket02a.nif");
	viewer::spotlight(rc);
	nif_test(meshes);
	dungeon = new Level("PadstowDungeon"); // <-- interior to load
	programLoop();
	return 1;
}
