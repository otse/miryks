#include <gloom/dark2.h>
#include <gloom/files.h>

#include <libs>

#include <algorithm>

namespace gloom
{
	const char *dataFolder = "Data/";

    Rc *loadRc(const char *prepend = "meshes\\", const char *path = "", unsigned long flags = 0x1)
	{
		std::string str = prepend;
		str += path;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
		Rc *rc = bsa_find_more(str.c_str(), flags);
		bsa_read(rc);
		return rc;
	}

	Nifp *loadNifp(Rc *rc, int save)
	{
		cassert(rc, "mh no rc");
		bsa_read(rc);
		Nifp *nif = malloc_nifp();
		nif->path = rc->path;
		nif->buf = rc->buf;
		nifp_read(nif);
		if (save)
			nifp_save(rc, nif);
		return nif;
	}

	Esp *loadEsp(const char *filename)
	{
		printf("loadEsp %s\n", filename);
		std::string path = pathToOldrim + dataFolder + filename;
		char *buf;
		int ret;
		// Try load plugin from skyrim else local folder
		(ret = fbuf(path.c_str(), &buf)) == -1 ? (ret = fbuf(filename, &buf)) : void();
		if (ret == -1)
		{
			printf("Couldn't find or otherwise load %s :(", filename);
			exit(1);
			return nullptr;
		}
		Esp *plugin = plugin_slate();
		plugin->name = filename;
		plugin->buf = buf;
		plugin->filesize = ret;
		plugin_load(plugin);
		return plugin;
	}

	Bsa *loadBsa(const char *filename)
	{
		std::string path = pathToOldrim + dataFolder + filename;
		printf("%s\n", path.c_str());
		if (exists(path.c_str()))
			return bsa_load(path.c_str());
		else
			return bsa_load(filename);
		return nullptr;
	}
}