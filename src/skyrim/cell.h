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

	Interior *get_interior(const char *, int = 5);

	WorldSpace *get_world_space(const char *, int = 5);

	extern Interior *dungeon;
	extern WorldSpace *worldSpace;

	class Cell : public Record
	{
	public:
		Grup<> persistent, temporary;
		uint16_t flags = 0;
		Cell(Record cell, Grup<> &grup) : Record(cell)
		{
			assertc(grup.hed().group_type == CellChildren);
			Grup first = grup.grup();
			Grup second = grup.grup();
			if (first.hed().group_type == CellPersistentChildren)
			{
				persistent = first;
				temporary = second;
			}
			else
				temporary = first;
			flags = *cell.data<uint16_t *>("DATA");
		}
	};

	class Interior : public Cell
	{
	public:
		Interior(Record, Grup<> &);
		~Interior();

		std::vector<Reference *> refs, labels, mstts;
		std::map<std::string, Reference *> edIds;

		Interior *Init();
		void Subgroup(Grup<>, int);
		void Unload();
		void put_cam_on_random_xmarker();
		void Update();

		const char *edId = nullptr;
		bool dontTeleport = false;
	};

	struct base
	{
		const char *word;
		record record;
		base(const char *word) : word(word)
		{
		}
	};

	struct wrld;

	struct wrld : public record
	{
		typedef wrld type;
		typedef grup<wrld> iter;
		grup<any> childs;
		wrld() {}
		wrld(iter &e)
		{
			(*this) = e;
		}
		void operator=(iter &e)
		{
			set(e.record());
			// record::operator = (e.record());
			childs = e.grup();
		}
	};

	class WorldSpace : public Record
	{
	public:
		Grup<> grup;
		std::vector<Exterior *> exteriors;
		std::vector<Reference *> references;
		WorldSpace(wrld &);
		WorldSpace(Record, Grup<>);
		WorldSpace(Grup<> &);
		WorldSpace *Init();
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
		Exterior(Record cell, Grup<> grup) : Cell(cell, grup)
		{
			worldSpace = nullptr;
			land = nullptr;
			xclc = data<XCLC *>("XCLC");
			assertc(xclc);
		}
		void Init();
		void Subgroup(Grup<>, int);
	};

	class Land : public Record
	{
	public:
		Exterior *exterior;
		Group *group;
		DrawGroup *drawGroup;
		Land(Record land);
	};

}