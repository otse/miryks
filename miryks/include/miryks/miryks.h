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
	void init_data_files();
	void init_archives();

	void view_in_place(resource *);

	extern char *editme;

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