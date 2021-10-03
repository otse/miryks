#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	Record get_world_space(const char *);
	Cell get_interior_cell(const char *, int);
	
	extern Interior *ginterior;

	struct Cell
	{
		Record wrcd;
		Grup persistent, temporary;
	};

	class Interior
	{
	public:
		static Interior *ginterior;

		Interior(const char *);
		~Interior();

		Cell cell;
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

	class Exterior
	{
	public:
		Exterior();
		~Exterior();

		void Load();
		void Unload();
	};

}