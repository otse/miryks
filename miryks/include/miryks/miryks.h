#pragma once

#include <miryks/list>
#include <miryks/libs.h>

#include <map>

#define ONE_SKYRIM_UNIT_IN_CM 1.428f
#define CM_TO_SKYRIM_UNITS (1.f / ONE_SKYRIM_UNIT_IN_CM)

#define EYE_HEIGHT 160 * CM_TO_SKYRIM_UNITS

namespace miryks
{
	namespace hooks
	{
	extern bool (*some_behavior)(int);
	}

	int init();
	void load_data_files();
	void load_archives();
	void load_interior(const char *);
	void load_world_space(const char *);

	void view_in_place(Res *);

	extern char *editme;

	template <int, typename>
	struct grup;
	struct grup_basic;

	struct record;
	struct record_basic;
	struct record_with_id;
	struct record_with_id_and_grup;

	typedef record_basic Record;

	class Char;
	class Player;
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

	record get_race(const char *, int);
	record_with_id_and_grup get_interior_cell(const char *, int);
}

namespace miryks
{
	Interior *get_interior(const char *, int);
	WorldSpace *get_world_space(const char *, int);

	namespace input
	{
	extern std::map<const char *, int> keys;
	bool pressing_key(const char *);
	bool holding_key(const char *);
	}
	
	extern bool showCursor;
	extern Player *player1;
	extern Monster *someDraugr, *meanSkelly;
	extern Char *someHuman;

	void reload_plugin();
	void reload_dungeon();
	void unset_dungeon();
	void third_person();
}