#pragma once

#include <skyrim/grup.h>

namespace skyrim
{
	extern char *editme;
	
	class SKRecord;
	class SKGrup;

	void ext_nif_save(const char *, NIF);
	NIF ext_nif_saved(const char *);
	std::map<const char *, NIF> &ext_nif_map();

	RES get_resource(
		const char *,
		const char * = "meshes\\",
		unsigned long = 0x1);

	NIF get_nif(const char *);

	ESP load_plugin(const char *, bool = false);
	BSA load_archive(const char *);

	void load_these_definitions(ESP);
	
	class SKKeyframes;
	SKKeyframes *load_keyframes_from_disk(const char *);

	const auto GetNif = get_nif;
	const auto GetResource = get_resource;
	const auto LoadPlugin = load_plugin;
	const auto LoadArchive = load_archive;
	
	class SKMesh;
	class SKMeshSkinned;
	class SKSkinnedMesh;

	class SKCharacter;
	class SKCreature;
	class SKSkeleton;
	class SKBone;
	class SKKeyframes;
	class SKAnimation;

	class SKInterior;
	class SKContainer;

	struct SKCell
	{
		SKRecord wrcd;
		SKGrup persistent, temporary;
	};
	SKRecord SkyrimGetRace(const char *);

	SKCell SkyrimGetCellInterior(const char *, int);

}