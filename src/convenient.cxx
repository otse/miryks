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

	Rc *load_rc(const char *prepend, const char *path, unsigned long flags)
	{
		std::string str = prepend;
		str += path;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		const char *s = str.c_str();
		Rc *rc = bsa_find_more(s, flags);
		if (!rc)
			printf("no rc at %s\n", s);
		bsa_read(rc);
		return rc;
	}

	Nif *load_nif(Rc *rc, bool cached)
	{
		cassert(rc, "load_nif 0 rc");
		Nif *nif;
		nif = nifp_saved(rc);
		if (cached && nif)
			return nif;
		bsa_read(rc);
		nif = malloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nifp_read(nif);
		if (cached)
			nifp_save(rc, nif);
		return nif;
	}

	Mesh *load_mesh(const char *model, bool cached)
	{
		// make a simple mesh, like a rock
		static std::map<const char *, Mesh *> meshes;
		if (meshes.count(model) && cached)
			return meshes[model];
		Rc *rc = load_rc("meshes\\", model, 0x1);
		if (!rc)
			return nullptr;
		Nif *nif = load_nif(rc, true);
		Mesh *mesh = new Mesh(nif);
		if (cached)
			meshes.emplace(model, mesh);
		return mesh;
	}

	Esp *load_plugin(const char *filename, bool essential)
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
			printf("couldn't find %s in /Data or /bin\n", filename);
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

	Bsa *load_archive(const char *filename)
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

	void View(Rc *rc)
	{
		static Mesh *mesh = nullptr;
		static DrawGroup *drawGroup = nullptr;
		if (mesh)
		{
			sceneDefault->drawGroups.Remove(drawGroup);
			delete mesh;
			delete drawGroup;
		}
		Nif *nif = load_nif(rc, false); // Note no use of cache
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