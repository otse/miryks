#pragma once

#include <dark/dark.h>
#include <dark/reference.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	class Cell;
	class Interior;
	class Exterior;
	class WorldSpace;
	class Land;

	Interior *GetInterior(const char *, int);

	WorldSpace *GetWorldSpace(const char *, int);

	extern Interior *dungeon;
	extern WorldSpace *gworldSpace;

	class Cell : public Record
	{
	public:
		Grup<> persistent, temporary;
		uint16_t flags = 0;
		Cell(Record cell, Grup<> &g) : Record(cell)
		{
			assertc(g.hed().group_type == CellChildren);

			if (g.grup().hed().group_type == CellPersistentChildren) {
				persistent = g.grup();
				g.next();
			}
			temporary = g.grup();
			flags = *cell.data<uint16_t *>("DATA");
		}
	};

	class Interior : public Cell
	{
	public:
		Interior(Record cell, Grup<> &g) : Cell(cell, g)
		{
		}
		~Interior();

		std::vector<Reference *> refs, labels, mstts;
		std::map<std::string, Reference *> edIds;

		void Init();
		void Subgroup(Grup<>, int);
		void Unload();
		void put_cam_on_random_xmarker();
		void Update();

		const char *edId = nullptr;
		bool dontTeleport = false;
	};

	class WorldSpace : public Record
	{
	public:
		Grup<> grup;
		std::vector<Exterior *> exteriors;
		std::vector<Reference *> references;
		WorldSpace(Record wrld, Grup<> g) : Record(wrld)
		{
			grup = g;
			printf("new WorldSpace: %s\n", data<const char *>("FULL"));
			formId xlcn = data<formId>("XLCN");
			int16_t *wctr = data<int16_t *>("WCTR");
			int32_t *nam0 = data<int32_t *>("NAM0");
			int32_t *nam9 = data<int32_t *>("NAM9");
		}
		void Init();
		void DiscoverAllCells();
		void LoadExterior(int, int);
	};

	struct XCLC
	{
		int32_t x, y, flags;
	};
	class Exterior : public Cell
	{
	public:
		WorldSpace *worldSpace;
		Land *land;
		XCLC *xclc;
		Exterior(Record cell, Grup<> g) : Cell(cell, g)
		{
			worldSpace = nullptr;
			land = nullptr;
			xclc = data<XCLC *>("XCLC");
			assertc(xclc);
		}
		void Init();
		void Subgroup(Grup<>, int);
	};

	struct VHGT
	{
		int32_t offset;
		char bytes[1089];
		char unknown[3];
		int end[ ];
	};
	class Land : public Record
	{
	public:
		Exterior *exterior;
		Land(Record land);
		VHGT *vhgt;
		Group *group;
		DrawGroup *drawGroup;
	};

}