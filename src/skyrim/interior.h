#pragma once

#include <dark/dark.h>
#include <dark/ref.h>

#include <skyrim/skyrim.h>
#include <skyrim/grup.h>
#include <skyrim/record.h>

using namespace dark;

namespace skyrim
{
	struct Cell
	{
		Record wrcd;
		Grup persistent, temporary;
	};

	Cell find_interior_cell(const char *, int);

	class Interior
	{
	public:
		Interior(const char *);
		~Interior();

		Cell cell;
		std::vector<Ref *> refs, labels, mstts;
		std::map<std::string, Ref *> edIds;

		void update();
		void subgroup(Grup, int);
		void load();
		void unload();
		void put_cam_on_random_xmarker();

		const char *edId = nullptr;
		bool alreadyTeleported = false;
	};

}