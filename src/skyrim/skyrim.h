#pragma once

#include <skyrim/grup.h>

namespace skyrim
{
	extern char *editme;
	
	class Record;
	class Grup;

	int ext_nif_save(const char *, Nif *);
	Nif *ext_nif_saved(const char *);
	std::map<const char *, Nif *> &ext_nif_map();

	Res *get_resource(const char *, const char * = "meshes\\", unsigned long = 0x1);
	Nif *get_nif(const char *);
	Esp *load_plugin(const char *, bool = false, bool = true);
	Bsa *load_archive(const char *);
	void load_these_definitions(Esp *);
	
	class Keyframes;
	Keyframes *load_keyframes_from_disk(const char *);
	
	class Mesh;
	class MeshSkinned;
	class SkinnedMesh;

	class Character;
	class Creature;
	class Skeleton;
	class Bone;
	class Keyframes;
	class Animation;

	class Interior;
	class Container;

	struct Cell
	{
		Record wrcd;
		Grup persistent, temporary;
	};

	Record skyrim_get_race(const char *);

	Cell skyrim_get_interior_cell(const char *, int);

}