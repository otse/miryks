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
		Record *record = nullptr;
		Grup *persistent = nullptr;
		Grup *temporary = nullptr;
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
		void parsegrup(int, CELL &, Grup *);
		void loadcell();
		void unload();
		void placecamera();
	};
}