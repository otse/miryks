

#include <dark/dark.h>

#include <core/files.h>

#include <skyrim/grup.h>
#include <skyrim/mesh.h>

#include <renderer/group.h>
#include <renderer/drawgroup.h>
#include <renderer/camera.h>
#include <renderer/scene.h>

#include <algorithm>

namespace dark
{
	std::map<void *, Nif *> nifs;

	int ext_nif_save(void *key, Nif *nif)
	{
		nifs.emplace(key, nif);
		return 1;
	}

	Nif *ext_nif_saved(void *key)
	{
		auto has = nifs.find(key);
		if (has != nifs.end())
			*has->second;
		return nullptr;
	}

	Nif *import_nif(Rc *rc, bool store)
	{
		assertm(rc, "import_nif null rc");
		Nif *nif;
		nif = ext_nif_saved(rc);
		if (store && nif)
			return nif;
		bsa_read(rc);
		nif = calloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nif_read(nif);
		if (store)
			ext_nif_save(rc, nif);
		return nif;
	}

	Rc *load_rc(const char *prepend, const char *path, unsigned long flags)
	{
		std::string str = prepend;
		str += path;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
					   { return std::tolower(c); });
		const char *s = str.c_str();
		Rc *rc = bsa_find_more(s, flags);
		if (rc == NULL)
			printf("no rc at %s\n", s);
		bsa_read(rc);
		return rc;
	}

	Mesh *create_simple_mesh_from_modl(const char *model, bool store)
	{
		static std::map<const char *, Mesh *> map;
		if (map.count(model) && store)
			return map[model];
		Rc *rc = load_rc("meshes\\", model, 0x1);
		if (rc == NULL)
			return nullptr;
		Nif *nif = import_nif(rc, true);
		Mesh *mesh = new Mesh(nif);
		if (store)
			map.emplace(model, mesh);
		return mesh;
	}

	Esp *load_plugin(const char *filename, bool essential)
	{
		printf("Load Plugin %s\n", filename);
		std::string path = editme + "/Data/" + filename;
		espp plugin;
		plugin = has_plugin(filename);
		if (plugin)
			return plugin;
		if (exists(path.c_str()))
			plugin = plugin_load(path.c_str());
		else if (exists(filename))
			plugin = plugin_load(filename);
		else
		{
			printf("couldn't find %s in /Data or /bin\n", filename);
			if (essential)
				exit(1);
			return nullptr;
		}
		load_definitions(plugin);
		return plugin;
	}

	void load_definitions(espp plugin)
	{
		static const auto things = {
			Statics,
			Lights,
			Doors,
			Furniture,
			Books,
			Containers,
			Armor,
			Weapons,
			Ammo,
			Misc,
			Alchemy,
			Ingredients,
			Mists,
			Plants,
		};
		for (const char *word : things)
			esp_check_grup(esp_top_grup(plugin, word));
	}

	Bsa *load_archive(const char *filename)
	{
		printf("Load Archive %s\n", filename);
		Bsa *bsa = bsa_get(filename);
		if (bsa)
			return bsa;
		std::string path = editme + "/Data/" + filename;
		if (exists(path.c_str()))
			return bsa_load(path.c_str());
		else if (exists(filename))
			return bsa_load(filename);
		else
		{
			printf("couldn't find %s in /Data or /bin\n", filename);
		}
		return nullptr;
	}
}