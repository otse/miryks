#pragma once

#include <dark/dark.h>
#include <dark/reference.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>
#include <skyrim/any_constellation.h>

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
		grup<any> persistent, temporary;
		uint16_t flags = 0;
		Cell(record, grup<>);
	};

	class Interior : public Cell
	{
	public:

		std::vector<Reference *> refs, labels, mstts;
		std::map<std::string, Reference *> edIds;

		Interior(constellation &);
		~Interior();
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

	class WorldSpace : public Record
	{
	public:
		grup<constellation> childs;
		std::vector<Exterior *> exteriors;
		std::vector<Reference *> references;
		WorldSpace(constellation &);
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
		Exterior(constellation temp) : Cell(temp.self, temp.childs)
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