#include <files.h>

#include <miryks/miryks.hpp>
#include <miryks/model.h>

#include <ogl/scene_graph.hpp>
#include <ogl/scene_graph.hpp>
#include <opengl/camera.h>
#include <opengl/scene.h>

#include <algorithm>

namespace miryks
{
	keyframes *load_keyframes_from_disk(const char *path)
	{
		if (nif *saved = got_ni(path))
			return new keyframes(saved);
		nif *model = calloc_ni();
		model->path = path;
		model->buf = get_binary_file(path);
		nif_read(model);
		save_ni(path, model);
		return new keyframes(model);
	}

	ESP load_plugin(const char *filename, bool loadInMemory)
	{
		//printf("Load Plugin %s\n", filename);
		if (strlen(filename) < 1)
			return NULL;
		std::string path = installed_path + "/Data/" + filename;
		if (ESP has = has_plugin(filename))
			return has;
		ESP plugin;
		if (exists(path.c_str()))
		{
			plugin = plugin_load(path.c_str(), loadInMemory);
			if (strstr(filename, ".esp"))
				printf("loading .esp from /Data\n");
		}
		else if (exists(filename))
		{
			plugin = plugin_load(filename, loadInMemory);
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
		std::string path = installed_path + "/Data/" + filename;
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
		static const auto things = {
			Activators,
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
			LandTextures,
			TextureSets
		};
		for (const char *word : things)
			esp_check_grup(esp_top(plugin, word));
	}
}