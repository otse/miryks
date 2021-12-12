#pragma once

#include <miryks/constants.h>
#include <miryks/libs.h>

#include <miryks/miryks.hpp>

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

	void view_in_place(Res *);

	extern char *editme;

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

	class keyframes;
	keyframes *load_keyframes_from_disk(const char *);
}

namespace miryks
{
	namespace input
	{
	extern std::map<const char *, int> keys;
	bool pressing_key(const char *);
	bool holding_key(const char *);
	}

	void reload_plugin();
	void reload_dungeon();
	void unset_dungeon();
	void third_person();
}