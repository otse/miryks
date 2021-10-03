#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	Interior *getInterior(const char *, int);

	WorldSpace *getWorldSpace(const char *, int);
	
	extern Interior *ginterior;
	extern WorldSpace *gworldSpace;

	class Cell
	{
	public:
		Record record;
		Grup persistent, temporary;
		Cell(Record, Grup);
	};

	class Interior : public Cell
	{
	public:
		Interior(Record r, Grup g) : Cell(r, g) {
			
		}
		~Interior();

		std::vector<Ref *> refs, labels, mstts;
		std::map<std::string, Ref *> edIds;

		void Update();
		void Subgroup(Grup, int);
		void Load();
		void Unload();
		void put_cam_on_random_xmarker();

		const char *edId = nullptr;
		bool alreadyTeleported = false;
	};

	class WorldSpace {
	public:
		Record record;
		Grup grup;
		WorldSpace(Record r, Grup g) {
			printf("WorldSpaced");
			record = r;
			grup = g;
		}
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