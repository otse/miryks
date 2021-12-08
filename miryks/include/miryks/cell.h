#pragma once

#include <miryks/miryks.h>
#include <miryks/reference.h>
#include <miryks/grup.h>
#include <miryks/record.h>

#include <opengl/renderer.h>

namespace miryks
{
	class Cell;
	class Interior;
	class Exterior;
	class WorldSpace;
	class Land;

	extern Interior *dungeon;
	extern WorldSpace *worldSpace;

	class Cell : public Record
	{
	public:
		uint16_t flags = 0;
		pt_grup<8> persistent;
		pt_grup<9> temporary;
		Cell(record_and_grup rng) : Record(rng)
		{
			assertc(rng.ghed().group_type == cell_children);
			grup_copy first, second;
			first = rng.next_grup();
			second = rng.next_grup();
			if (first.ghed().group_type == cell_persistent_children)
			{
				persistent = first;
				temporary = second;
			}
			else
				temporary = first;
			flags = *rng.data<uint16_t *>("DATA");
		}
	};

	class Interior : public Cell
	{
	public:
		std::vector<Reference *> refs, labels, mstts;
		std::map<std::string, Reference *> edIds;

		Interior(record_and_grup);
		~Interior();
		Interior *Init();
		void Unload();
		void PutCam();
		void Update();

		const char *edId = nullptr;
		bool dontTeleport = false;
	};

	class WorldSpace : public Record
	{
	public:
		grup_basic childs;
		std::vector<Exterior *> exteriors;
		std::vector<Reference *> references;
		WorldSpace(record_and_grup);
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
		Exterior(record_and_grup);
		void Init();
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