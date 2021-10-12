#pragma once

#include <lib.h>
#include <map>

#define ONE_SKYRIM_UNIT_IN_CM 1.428f
#define CM_TO_SKYRIM_UNITS (1.f / ONE_SKYRIM_UNIT_IN_CM)

#define EYE_HEIGHT 160 * CM_TO_SKYRIM_UNITS

namespace skyrim
{
	extern char *editme;

	class Record;

	class Char;
	class Monster;
	class Skel;
	class Bone;
	class Keyf;
	class Anim;

	class Model;
	class ModelSkinned;
	class SkinnedMesh;

	class Cell;
	class Interior;
	class WorldSpace;
	class Exterior;
	
	class Container;

	void save_nif(const char *, Nif *);
	Nif *saved_nif(const char *);
	std::map<const char *, Nif *> &get_nifs();

	Res *get_res(
		const char *,
		const char * = "meshes\\",
		unsigned long = 0x1);

	Nif *get_nif(Res *);
	Nif *get_nif(const char *);

	ESP load_plugin(const char *, bool = false);
	BSA load_archive(const char *);

	void load_these_definitions(ESP);

	Keyf *load_keyframes_from_disk(const char *);

	Record GetRace(const char *);

}