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
		grup<> persistent, temporary;
		uint16_t flags = 0;
		Cell(record &, grup<> &);
	};

	class Interior : public Cell
	{
	public:
		std::vector<Reference *> refs, labels, mstts;
		std::map<std::string, Reference *> edIds;

		Interior(record_with_id_and_grup);
		~Interior();
		Interior *Init();
		void Sift(grup<> &, int);
		void Unload();
		void PutCam();
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
		grup<> childs;
		std::vector<Exterior *> exteriors;
		std::vector<Reference *> references;
		WorldSpace(record_with_id_and_grup &);
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
		Exterior(record_with_id_and_grup &);
		void Init();
		void Subgroup(grup<> &, int);
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