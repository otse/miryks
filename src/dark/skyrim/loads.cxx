#include <libs>
#include <dark/dark.h>
#include <dark/files.h>

#include <dark/skyrim/object.h>
#include <dark/skyrim/mesh.h>

#include <opengl/group.h>
#include <opengl/drawgroup.h>
#include <opengl/camera.h>
#include <opengl/scene.h>

#include <algorithm>

// these importers use the c at /lib

namespace dark
{
	const char *dataFolder = "Data/";

	Rc *load_rc(const char *prepend, const char *path, unsigned long flags)
	{
		std::string str = prepend;
		str += path;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
					   { return std::tolower(c); });
		const char *s = str.c_str();
		Rc *rc = bsa_find_more(s, flags);
		if (!rc)
			printf("no rc at %s\n", s);
		bsa_read(rc);
		return rc;
	}

	Nif *import_nif(Rc *rc, bool store)
	{
		cassert(rc, "import_nif null rc");
		Nif *nif;
		nif = nifp_saved(rc);
		if (store && nif)
			return nif;
		bsa_read(rc);
		nif = malloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nifp_read(nif);
		if (store)
			nifp_save(rc, nif);
		return nif;
	}

	Mesh *create_mesh(const char *model, bool store)
	{
		// make a simple mesh, like a rock
		static std::map<const char *, Mesh *> meshes;
		if (meshes.count(model) && store)
			return meshes[model];
		Rc *rc = load_rc("meshes\\", model, 0x1);
		if (!rc)
			return nullptr;
		Nif *nif = import_nif(rc, true);
		Mesh *mesh = new Mesh(nif);
		if (store)
			meshes.emplace(model, mesh);
		return mesh;
	}

	Esp *load_plugin(const char *filename, bool essential)
	{
		printf("Load Plugin %s\n", filename);
		std::string path = editme + dataFolder + filename;
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
		std::string path = editme + dataFolder + filename;
		if (exists(path.c_str()))
			return bsa_load(path.c_str());
		else if (exists(filename))
			return bsa_load(filename);
		return nullptr;
	}
}