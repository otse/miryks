#pragma once

#include <dark/dark.h>
#include <dark/ref.h>
#include <skyrim/skyrim.h>

using namespace dark;

namespace skyrim
{
	struct CELL
	{
		bool good = false;
		recordp record = nullptr;
		grupp persistent = nullptr;
		grupp temporary = nullptr;
	};

	class Interior
	{
	public:
		Interior(const char *);
		~Interior();

		const char *editorId;
		bool alreadyTeleported = false;

		CELL loaded_cell;
		std::vector<Ref *> refs, iterables;
		std::map<std::string, Ref *> editorIds;

		static CELL getcell(const char *);
		void update();
		void parsegrup(int, CELL &, grupp);
		void loadcell();
		void unload();
		void placecamera();
	};
}