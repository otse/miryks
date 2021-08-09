#include <lib.h>

#include <dark/dark.h>

#include <core/files.h>

#include <skyrim/grup.h>
#include <skyrim/mesh.h>

#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/camera.h>
#include <renderer/scene.h>

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
		if (rc==NULL)
			printf("no rc at %s\n", s);
		bsa_read(rc);
		return rc;
	}

	Nif *import_nif(Rc *rc, bool store)
	{
		assertm(rc, "import_nif null rc");
		Nif *nif;
		nif = nifp_saved(rc);
		if (store && nif)
			return nif;
		bsa_read(rc);
		nif = calloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nifp_read(nif);
		if (store)
			nifp_save(rc, nif);
		return nif;
	}

	Mesh *create_simple_mesh_from_modl(const char *model, bool store)
	{
		static std::map<const char *, Mesh *> simple_meshes;
		if (simple_meshes.count(model) && store)
			return simple_meshes[model];
		Rc *rc = load_rc("meshes\\", model, 0x1);
		if (rc==NULL)
			return nullptr;
		Nif *nif = import_nif(rc, true);
		Mesh *mesh = new Mesh(nif);
		if (store)
			simple_meshes.emplace(model, mesh);
		return mesh;
	}

	espp load_plugin(const char *filename, bool essential)
	{
		printf("Load Plugin %s\n", filename);
		std::string path = editme + dataFolder + filename;
		char *buf;
		int ret;
		espp esp;
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