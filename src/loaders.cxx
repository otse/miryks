#include <libs>
#include <Gloom/Gloom.h>
#include <Gloom/Object.h>
#include <Gloom/Mesh.h>
#include <Gloom/Files.h>

#include <OpenGL/Group.h>
#include <OpenGL/DrawGroup.h>
#include <OpenGL/Camera.h>
#include <OpenGL/Scene.h>

#include <algorithm>

namespace gloom
{
	const char *dataFolder = "Data/";

	Resource *loadResource(const char *prepend = "meshes\\", const char *path = "", unsigned long flags = 0x1)
	{
		std::string str = prepend;
		str += path;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		const char *s = str.c_str();
		Resource *rc = bsa_find_more(s, flags);
		if (!rc)
			printf("no rc at %s\n", s);
		bsa_read(rc);
		return rc;
	}

	Nif *loadNif(Resource *rc, bool useCache)
	{
		cassert(rc, "no rc nif loader");
		Nif *nif;
		nif = nifp_saved(rc);
		if (useCache && nif != NULL)
			return nif;
		bsa_read(rc);
		nif = malloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nifp_read(nif);
		if (useCache)
			nifp_save(rc, nif);
		return nif;
	}

	Mesh *loadMesh(const char *model, bool useCache)
	{
		static std::map<const char *, Mesh *> meshes;
		if (meshes.count(model) && useCache)
			return meshes[model];
		Resource *rc = loadResource("meshes\\", model, 0x1);
		if (!rc)
			return nullptr;
		Nif *nif = loadNif(rc, true);
		Mesh *mesh = new Mesh(nif);
		if (useCache)
			meshes.emplace(model, mesh);
		return mesh;
	}

	Esp *loadPlugin(const char *filename, bool essential)
	{
		printf("Load Plugin %s\n", filename);
		std::string path = pathToOldrim + dataFolder + filename;
		char *buf;
		int ret;
		Esp *esp;
		esp = has_plugin(filename);
		if (esp)
			return esp;
		if ((ret = fbuf(path.c_str(), &buf)) == -1)
			ret = fbuf(filename, &buf);
		if (ret == -1)
		{
			printf("couldn't find %s anywhere\n", filename);
			if (essential)
				exit(1);
			return nullptr;
		}
		esp = plugin_slate();
		esp->name = filename;
		esp->buf = buf;
		esp->filesize = ret;
		plugin_load(esp);
		return esp;
	}

	Bsa *loadArchive(const char *filename)
	{
		printf("Load Archive %s\n", filename);
		Bsa *bsa = bsa_get(filename);
		if (bsa)
			return bsa;
		std::string path = pathToOldrim + dataFolder + filename;
		if (exists(path.c_str()))
			return bsa_load(path.c_str());
		else if (exists(filename))
			return bsa_load(filename);
		return nullptr;
	}

	void View(Resource *rc)
	{
		static Mesh *mesh = nullptr;
		static DrawGroup *drawGroup = nullptr;
		if (mesh)
		{
			sceneDefault->drawGroups.Remove(drawGroup);
			delete mesh;
			delete drawGroup;
		}
		Nif *nif = loadNif(rc, false); // Note no use of cache
		nifp_save(rc, nif);
		mesh = new Mesh(nif);
		drawGroup = new DrawGroup(mesh->baseGroup, translate(mat4(1.0), firstPersonCamera->pos));
		sceneDefault->drawGroups.Add(drawGroup);
		HideCursor();
		cameraCurrent = panCamera;
		panCamera->pos = drawGroup->aabb.center();
		//panCamera->pos = firstPersonCamera->pos;
		panCamera->radius = drawGroup->aabb.radius2() * 2;
	}
} // namespace gloom