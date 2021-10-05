#pragma once

#include <dark/dark.h>
#include <dark/reference.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	Interior *GetInterior(const char *, int);

	WorldSpace *GetWorldSpace(const char *, int);
	
	extern Interior *dungeon;
	extern WorldSpace *gworldSpace;

	class Cell : public Record
	{
	public:
		Grup<> persistent, temporary;
		Cell(Record, Grup<>);
	};

	class Interior : public Cell
	{
	public:
		Interior(Record cell, Grup<> g) : Cell(cell, g) {
			
		}
		~Interior();

		std::vector<Reference *> refs, labels, mstts;
		std::map<std::string, Reference *> edIds;

		void Update();
		void Subgroup(Grup<>, int);
		void Load();
		void Unload();
		void put_cam_on_random_xmarker();

		const char *edId = nullptr;
		bool alreadyTeleported = false;
	};

	class WorldSpace : public Record {
	public:
		Grup<> grupw;
		WorldSpace(Record wrld, Grup<> g) : Record(wrld) {
			grupw = g;
			printf("new WorldSpace: %s\n", data<const char *>("FULL"));
			formId xlcn = data<formId>("XLCN");
			int16_t *wctr = data<int16_t *>("WCTR");
			int32_t *nam0 = data<int32_t *>("NAM0");
			int32_t *nam9 = data<int32_t *>("NAM9");
		}
		void Load();
	};

	class Exterior
	{
	public:
		Exterior();
		~Exterior();

		void Load();
		void Unload();
	};

}