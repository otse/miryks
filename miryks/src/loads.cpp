#include <files.h>

#include <miryks/miryks.hpp>
#include <miryks/model.h>

#include <opengl/group.h>
#include <opengl/drawgroup.h>
#include <opengl/camera.h>
#include <opengl/scene.h>

#include <algorithm>

namespace miryks
{
	std::map<const char *, Nif *> nifs;

	std::map<const char *, Nif *> &get_nifs()
	{
		return nifs;
	}

	void save_nif(const char *key, Nif *nif)
	{
		nifs.emplace(key, nif);
	}

	Nif *saved_nif(const char *key)
	{
		auto has = nifs.find(key);
		if (has != nifs.end())
			*has->second;
		return nullptr;
	}

	Res *get_res(
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

	Nif *get_nif(const char *modl)
	{
		if (!modl)
			return nullptr;
		return get_nif(get_res(modl));
	}

	Nif *get_nif(Res *res)
	{
		if (!res)
			return nullptr;
		Nif *saved = saved_nif(res->path);
		if (saved)
			return saved;
		bsa_read(res);
		Nif *model = calloc_nifp();
		model->path = res->path;
		model->buf = res->buf;
		nif_read(model);
		save_nif(res->path, model);
		return model;
	}

	keyframes *load_keyframes_from_disk(const char *path)
	{
		if (Nif *saved = saved_nif(path))
			return new keyframes(saved);
		Nif *model = calloc_nifp();
		model->path = path;
		int len = fbuf(path, &model->buf, false);
		nif_read(model);
		save_nif(path, model);
		return new keyframes(model);
	}

	ESP load_plugin(const char *filename, bool whole)
	{
		//printf("Load Plugin %s\n", filename);
		if (strlen(filename) < 1)
			return NULL;
		std::string path = std::string(editme) + "/Data/" + filename;
		if (ESP has = has_plugin(filename))
			return has;
		ESP plugin;
		if (exists(path.c_str()))
		{
			plugin = plugin_load(path.c_str(), whole);
			//if (strstr(filename, ".esp"))
			//	printf("loading .esp from /Data\n");
		}
		else if (exists(filename))
		{
			plugin = plugin_load(filename, whole);
		}
		else
		{
			printf("couldn't find %s in /Data or /bin\n", filename);
			//exit(1);
			return NULL;
		}
		load_these_definitions(plugin);
		return plugin;
	}

	BSA load_archive(const char *filename)
	{
		//printf("Load Archive %s\n", filename);
		BSA bsa = bsa_get(filename);
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

	void load_these_definitions(ESP plugin)
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
			Flora,
		};
		for (const char *word : things)
			esp_check_grup(esp_top(plugin, word));
	}
}