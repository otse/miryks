

#include <dark/dark.h>
#include <dark/files.h>

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

	Rc *load_resource(
		const char *path, const char *prepend, unsigned long flags)
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

	Keyframes *load_keyframes_from_disk(const char *path)
	{
		Nif *nif = calloc_nifp();
		nif->path = path;
		char *lvalue = (char *)nif->buf;
		fbuf(path, &lvalue);
		nif_read(nif);
		ext_nif_save(nif, nif);
		Keyframes *keyframes = new Keyframes(nif);
		return keyframes;
	}

	Nif *load_model(Rc *rc, bool useStore)
	{
		assertm(rc, "import_nif null rc");
		Nif *nif, *saved;
		nif = ext_nif_saved(rc);
		if (useStore && nif)
			return nif;
		bsa_read(rc);
		nif = calloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nif_read(nif);
		if (useStore)
			ext_nif_save(rc->path, nif);
		return nif;
	}

	Mesh *create_simple_mesh_from_modl(const char *model, bool useStore)
	{
		static std::map<const char *, Mesh *> map;
		if (map.count(model) && useStore)
			return map[model];
		Rc *rc = load_resource(model);
		if (rc == NULL)
			return nullptr;
		Nif *nif = load_model(rc, true);
		Mesh *mesh = new Mesh(nif);
		if (useStore)
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
		load_these_definitions(plugin);
		return plugin;
	}

	void load_these_definitions(espp plugin)
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