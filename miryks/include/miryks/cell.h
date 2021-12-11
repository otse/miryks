#pragma once

#include <miryks/miryks.h>
#include <miryks/reference.h>
#include <miryks/grup.h>
#include <miryks/record.h>

#include <opengl/renderer.h>

namespace miryks
{
	class cell;
	class interior;
	class mir_exterior;
	class mir_worldspace;
	class land;

	extern interior *dungeon;
	extern mir_worldspace *worldSpace;

	class cell : public record_copy
	{
	public:
		uint16_t flags;
		grup_copy persistent, temporary;
		cell(rgrup_copy rg) : record_copy(rg)
		{
			flags = 0;
			assertc(rg.ghed().group_type == cell_children);
			grup_copy first, second;
			first = rg.next_grup();
			second = rg.next_grup();
			if (first.ghed().group_type == cell_persistent_children)
			{
				persistent = first;
				temporary = second;
			}
			else
				temporary = first;
			flags = *rg.data<uint16_t *>("DATA");
		}
	};

	class interior : public cell
	{
	public:
		std::vector<reference *> refs, labels, mstts;
		std::map<std::string, reference *> edIds;

		interior(rgrup_copy);
		~interior();
		interior *Init();
		void Unload();
		void PutCam();
		void Update();

		const char *edId = nullptr;
		bool dontTeleport = false;
	};

	class mir_worldspace : public record_copy
	{
	public:
		grup_copy childs;
		std::vector<mir_exterior *> exteriors;
		std::vector<reference *> references;
		mir_worldspace(rgrup_copy);
		mir_worldspace *Init();
		void DiscoverAllCells();
		void LoadExterior(int, int);
	};

	struct XCLC
	{
		int32_t x, y, flags;
	};
	class mir_exterior : public cell
	{
	public:
		mir_worldspace *worldSpace;
		land *land;
		XCLC *xclc;
		mir_exterior(rgrup_copy);
		void Init();
	};

	class land : public record_copy
	{
	public:
		mir_exterior *exterior;
		group_type *group;
		drawgroup *drawGroup;
		land(record_copy land);
	};

}