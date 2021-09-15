

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
	std::map<const char *, Nif *> nifs;

	std::map<const char *, Nif *> &ext_nif_map()
	{
		return nifs;
	}

	int ext_nif_save(const char *key, Nif *nif)
	{
		nifs.emplace(key, nif);
		return 1;
	}

	Nif *ext_nif_saved(const char *key)
	{
		auto has = nifs.find(key);
		if (has != nifs.end())
			*has->second;
		return nullptr;
	}

	Res *load_res(
		const char *path, const char *prepend, unsigned long flags)
	{
		std::string str = prepend;
		str += path;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c)
					   { return std::tolower(c); });
		const char *s = str.c_str();
		Res *res = bsa_find_more(s, flags);
		if (res == NULL)
			printf("no res at %s\n", s);
		bsa_read(res);
		return res;
	}

	Keyframes *load_keyframes_from_disk(const char *path)
	{
		if (Nif *saved = ext_nif_saved(path))
			return new Keyframes(saved);
		Nif *model = calloc_nifp();
		model->path = path;
		int len = fbuf(path, &model->buf);
		nif_read(model);
		ext_nif_save(path, model);
		return new Keyframes(model);
	}

	Nif *load_model(Res *res)
	{
		if (res == NULL)
			return nullptr;
		if (Nif *saved = ext_nif_saved(res->path))
			return saved;
		bsa_read(res);
		Nif *model = calloc_nifp();
		model->path = res->path;
		model->buf = res->buf;
		nif_read(model);
		ext_nif_save(res->path, model);
		return model;
	}

	Esp *load_plugin(const char *filename, bool mem, bool essential)
	{
		printf("Load Plugin %s\n", filename);
		std::string path = std::string(editme) + "/Data/" + filename;
		Esp *plugin;
		if (Esp *has = has_plugin(filename))
			return has;
		if (exists(path.c_str()))
			plugin = plugin_load(path.c_str(), mem);
		else if (exists(filename))
			plugin = plugin_load(filename, mem);
		else
		{
			if (essential) {
				// imgui invoked loads would be non essential
				printf("couldn't find %s in /Data or /bin\n", filename);
				exit(1);
			}
			return nullptr;
		}
		load_these_definitions(plugin);
		return plugin;
	}

	Bsa *load_archive(const char *filename)
	{
		printf("Load Archive %s\n", filename);
		Bsa *bsa = bsa_get(filename);
		if (bsa)
			return bsa;
		std::string path = std::string(editme) + "/Data/" + filename;
		if (exists(path.c_str()))
		{
			return bsa_load(path.c_str());
		}
		else if (exists(filename))
		{
			return bsa_load(filename);
		}
		else
		{
			printf("couldn't find %s in /Data or /bin\n", filename);
		}
		return nullptr;
	}

	void load_these_definitions(Esp *plugin)
	{
		// we only discovered top grups at this point,
		// we need to build the objects within by "checking" them
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
}